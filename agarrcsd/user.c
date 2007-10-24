/*
 * Copyright (c) 2005-2007 Hypertriton, Inc. <http://hypertriton.com/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <agar/core/strlcpy.h>
#include <agar/core/strlcat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <dirent.h>
#include <signal.h>

#include "agarrcsd.h"
#include "pathnames.h"

#include <agar/gui.h>

const AG_ObjectOps UserOps = {
	"User",
	sizeof(User),
	{ 8, 0 },
	UserInit,
	NULL,		/* reinit */
	NULL,		/* destroy */
	UserLoad,
	UserSave,
	UserEdit
};

const char *UserLangDefault = "en";

void
UserLink(User *u)
{
	AG_ObjectAttach(agWorld, u);
}

void
UserUnlink(User *u)
{
	AG_ObjectDetach(u);
}

User *
UserLookup(const char *name)
{
	return (AG_ObjectFindF("/%s", name));
}

void
UserInit(void *p, const char *name)
{
	User *u = p;

	AG_ObjectInit(u, name, &UserOps);
	u->name[0] = '\0';
	u->pass[0] = '\0';
	u->real_name[0] = '\0';
	u->email[0] = '\0';
	u->lang[0] = 'e';
	u->lang[1] = 'n';
	u->lang[2] = '\0';
	u->country[0] = '\0';
	u->comments[0] = '\0';
	u->flags = 0;
	strlcpy(u->lang, "en", sizeof(u->lang));
}

int
UserLoad(void *p, AG_Netbuf *buf)
{
	User *u = p;
	Uint32 i, count;
	
	if (AG_ReadObjectVersion(buf, u, NULL) == -1)
		return (-1);

	AG_CopyString(u->name, buf, sizeof(u->name));
	AG_CopyString(u->pass, buf, sizeof(u->pass));
	AG_CopyString(u->real_name, buf, sizeof(u->real_name));
	AG_CopyString(u->email, buf, sizeof(u->email));
	AG_CopyString(u->lang, buf, sizeof(u->lang));
	AG_CopyString(u->country, buf, sizeof(u->country));
	AG_CopyString(u->comments, buf, sizeof(u->comments));
	u->flags = (Uint)AG_ReadUint32(buf);
	return (0);
}

int
UserSave(void *p, AG_Netbuf *buf)
{
	User *u = p;
	FILE *f;

	AG_WriteObjectVersion(buf, u);
	AG_WriteString(buf, u->name);
	AG_WriteString(buf, u->pass);
	AG_WriteString(buf, u->real_name);
	AG_WriteString(buf, u->email);
	AG_WriteString(buf, u->lang);
	AG_WriteString(buf, u->country);
	AG_WriteString(buf, u->comments);
	AG_WriteUint32(buf, (Uint32)u->flags);
	return (0);
}

static int
UserNameValid(const char *name)
{
	const char *c = name;

	for (c = name; *c != '\0'; c++) {
		if (c == name && !isalpha(*c)) {
			AG_SetError("Username must begin with an alphabetic "
			           "character.");
			return (0);
		}
		if (!isalnum(*c) && *c != '_' && *c != '.') {
			AG_SetError("Username is limited to alphanumeric "
			           "characters, underscores and periods.");
			return (0);
		}
		if (*c == '.') {
			if (c[1] == '.') {
				AG_SetError("Username cannot contain two "
				            "consecutive periods.");
				return (0);
			} else {
				AG_SetError("Usernames cannot end with a "
				            "period.");
				return (0);
			}
		}
	}
	return (1);
}

static int
UserEmailValid(const char *name)
{
	const char *c = name;
	int at = 0;

	if (name[0] == '\0') {
		AG_SetError("E-mail address was not specified.");
		return (0);
	}
	for (c = name; *c != '\0'; c++) {
		if (*c == '@')
			at++;
	}
	if (at != 1) {
		AG_SetError("Malformed e-mail address");
		return (0);
	}
	/* TODO more checks */
	return (1);
}

static int
SetInfosFromCommand(NS_Command *cmd, User *u)
{
	char *pass = NS_CommandString(cmd, "pass");
	char *real_name = NS_CommandString(cmd, "real_name");
	char *email = NS_CommandString(cmd, "email");
	char *lang = NS_CommandString(cmd, "lang");
	char *country = NS_CommandString(cmd, "country");
	Uint flags = 0;
	
	NS_CommandInt(cmd, "flags", &flags);

	if (real_name == NULL || real_name[0] == '\0' ||
	    pass == NULL || pass[0] == '\0' ||
	    email == NULL || email[0] == '\0' ||
	    country == NULL || country[0] == '\0') {
		AG_SetError("Missing parameter.");
		return (-1);
	}
	if (!UserEmailValid(u->email))
		return (-1);

	strlcpy(u->pass, pass, sizeof(u->pass));
	strlcpy(u->real_name, real_name, sizeof(u->real_name));
	strlcpy(u->email, email, sizeof(u->email));
	strlcpy(u->country, country, sizeof(u->country));
	NS_CommandCopyString(u->comments, cmd, "comments", sizeof(u->comments));

	if (lang != NULL && lang[0] != '\0' && strlen(lang) == 2) {
		strlcpy(u->lang, lang, sizeof(u->lang));
	} else {
		strlcpy(u->lang, UserLangDefault, sizeof(u->lang));
	}

	u->flags &= ~(USER_SETTABLE_FLAGS);
	u->flags |= flags & USER_SETTABLE_FLAGS;
	return (0);
}

/* Obtain user information. */
int
user_get_infos(NS_Command *cmd, void *p)
{
	char *name = NS_CommandString(cmd, "name");
	User *u;
	
	if ((u = UserLookup(name)) == NULL)
		return (-1);

	printf("0 ");
	printf("name=%s:", u->name);
	printf("real_name=%s:", u->real_name);
	printf("email=%s:", u->email);
	printf("lang=%s:", u->lang);
	printf("country=%s:", u->country);
	printf("comments=%s:", u->comments);
	printf("flags=%x:", u->flags);
	printf("\n");
	return (0);
}

/* Set user information. */
int
user_set_infos(NS_Command *cmd, void *p)
{
	char *name = NS_CommandString(cmd, "name");
	User *u;

	if ((u = UserLookup(name)) == NULL) {
		return (-1);
	}
	return (SetInfosFromCommand(cmd, u));
}

int
user_show(NS_Command *cmd, void *p)
{
	User *u;
	int nmatches = 0;

	fputs("0 ", stdout);
	AGOBJECT_FOREACH_CLASS(u, agWorld, user, "User:*") {
		fputs(u->name, stdout);
		fputc(':', stdout);
		nmatches++;
	}
	fputc('\n', stdout);

	NS_Log(NS_DEBUG, "%d users", nmatches);
	if (nmatches == 0) {
		AG_SetError("The user list is empty.");
		return (-1);
	}
	return (0);
}

/* Activate a new account (admin). */
int
user_activate(NS_Command *cmd, void *p)
{
	char *name = NS_CommandString(cmd, "name");
	Uint flags = 0;
	unsigned int h;
	User *u;

	NS_CommandInt(cmd, "flags", &flags);

	if (name == NULL || name[0] == '\0') {
		AG_SetError("Username was not specified.");
		return (-1);
	}
	if (!UserNameValid(name)) {
		return (-1);
	}
	if (UserLookup(name) != NULL) {
		AG_SetError("Username `%s' is already taken.", name);
		return (-1);
	}

	u = Malloc(sizeof(User), M_OBJECT);
	UserInit(u, name);
	strlcpy(u->name, name, sizeof(u->name));
	if (SetInfosFromCommand(cmd, u) == -1)
		goto fail;

	if (AG_ObjectSave(u) == -1) {
		goto fail;
	}
	UserLink(u);

	NS_Log(NS_INFO, "Activated account `%s'.", u->name);
	printf("0 ok\n");

	/* Rehash the user table. */
	kill(getppid(), SIGHUP);
	return (0);
fail:
	AG_ObjectDestroy(u);
	Free(u, M_OBJECT);
	return (-1);
}

void *
UserEdit(void *p)
{
	User *u = p;
	AG_Window *win;
	AG_Textbox *tb;
	AG_Checkbox *cb;

	win = AG_WindowNew(0);
	AG_WindowSetCaption(win, "User: %s", AGOBJECT(u)->name);
	AG_WindowSetPosition(win, AG_WINDOW_LOWER_CENTER, 1);

	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL|AG_TEXTBOX_FOCUS, "Name: ");
	AG_WidgetBindString(tb, "string", u->name, sizeof(u->name));

	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "Password: ");
	AG_TextboxSetPassword(tb, 1);
	AG_WidgetBindString(tb, "string", u->pass, sizeof(u->pass));
	
	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "Real name: ");
	AG_WidgetBindString(tb, "string", u->real_name, sizeof(u->real_name));
	
	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "E-mail address: ");
	AG_WidgetBindString(tb, "string", u->email, sizeof(u->email));
	
	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "Language (ISO639): ");
	AG_WidgetBindString(tb, "string", u->lang, sizeof(u->lang));
	
	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "Country (ISO3166): ");
	AG_WidgetBindString(tb, "string", u->country, sizeof(u->country));
	
	tb = AG_TextboxNew(win, AG_TEXTBOX_HFILL, "Comments: ");
	AG_WidgetBindString(tb, "string", u->comments, sizeof(u->comments));

	cb = AG_CheckboxNew(win, 0, "Write access");
	AG_WidgetBindFlag(cb, "state", &u->flags, USER_WRITE);
	
	cb = AG_CheckboxNew(win, 0, "Administrative access");
	AG_WidgetBindFlag(cb, "state", &u->flags, USER_ADMIN);
	
	cb = AG_CheckboxNew(win, 0, "Send e-mail notices");
	AG_WidgetBindFlag(cb, "state", &u->flags, USER_EMAIL_NOTICES);

	return (win);
}
