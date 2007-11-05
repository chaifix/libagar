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

#include <core/core.h>

#include "separator.h"
#include "window.h"
#include "primitive.h"

AG_Separator *
AG_SeparatorNew(void *parent, enum ag_separator_type type)
{
	AG_Separator *sep;

	sep = Malloc(sizeof(AG_Separator), M_OBJECT);
	AG_SeparatorInit(sep, type, 1);
	AG_ObjectAttach(parent, sep);
	return (sep);
}

AG_Separator *
AG_SeparatorNewInv(void *parent, enum ag_separator_type type)
{
	AG_Separator *sep;

	sep = Malloc(sizeof(AG_Separator), M_OBJECT);
	AG_SeparatorInit(sep, type, 0);
	AG_ObjectAttach(parent, sep);
	return (sep);
}

void
AG_SeparatorInit(AG_Separator *sep, enum ag_separator_type type, int visible)
{
	AG_WidgetInit(sep,
	    visible ? &agSeparatorOps : &agSeparatorInvisibleOps,
	    (type == AG_SEPARATOR_HORIZ) ? AG_WIDGET_HFILL : AG_WIDGET_VFILL);
	sep->type = type;
	sep->padding = 4;
	sep->visible = visible;
}

static void
SizeRequest(void *p, AG_SizeReq *r)
{
	AG_Separator *sep = p;

	r->w = sep->padding*2 + 2;
	r->h = sep->padding*2 + 2;
}

static int
SizeAllocate(void *p, const AG_SizeAlloc *a)
{
	AG_Separator *sep = p;

	if (a->w < sep->padding*2 + 2 ||
	    a->h < sep->padding*2 + 2) {
		return (-1);
	}
	return (0);
}

static void
Draw(void *p)
{
	AG_Separator *sep = p;

	switch (sep->type) {
	case AG_SEPARATOR_HORIZ:
		STYLE(sep)->SeparatorHoriz(sep);
		break;
	case AG_SEPARATOR_VERT:
		STYLE(sep)->SeparatorVert(sep);
		break;
	}
}

void
AG_SeparatorSetPadding(AG_Separator *sep, Uint pixels)
{
	sep->padding = pixels;
}

const AG_WidgetOps agSeparatorOps = {
	{
		"AG_Widget:AG_Separator",
		sizeof(AG_Separator),
		{ 0,0 },
		NULL,		/* init */
		NULL,		/* free */
		NULL,		/* destroy */
		NULL,		/* load */
		NULL,		/* save */
		NULL		/* edit */
	},
	Draw,
	SizeRequest,
	SizeAllocate
};
const AG_WidgetOps agSeparatorInvisibleOps = {
	{
		"AG_Widget:AG_Separator",
		sizeof(AG_Separator),
		{ 0,0 },
		NULL,		/* init */
		NULL,		/* free */
		NULL,		/* destroy */
		NULL,		/* load */
		NULL,		/* save */
		NULL		/* edit */
	},
	NULL,			/* draw */
	SizeRequest,
	SizeAllocate
};
