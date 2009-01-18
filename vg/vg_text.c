/*
 * Copyright (c) 2004-2008 Hypertriton, Inc. <http://hypertriton.com/>
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

/*
 * Text entity.
 */

#include <core/core.h>
#include <core/config.h>

#include <gui/widget.h>
#include <gui/primitive.h>
#include <gui/opengl.h>
#include <gui/textbox.h>
#include <gui/font_selector.h>
#include <gui/checkbox.h>

#include "vg.h"
#include "vg_view.h"
#include "icons.h"

#include <stdarg.h>
#include <string.h>

static void
Init(void *p)
{
	VG_Text *vt = p;

	vt->text[0] = '\0';
	vt->p1 = NULL;
	vt->p2 = NULL;
	vt->align = VG_ALIGN_MC;
	vt->nPtrs = 0;
	vt->fontSize = agGUI ? agDefaultFont->size : 12;
	vt->fontFlags = agGUI ? agDefaultFont->flags : 0;
	vt->fontFace[0] = '\0';
}

static int
Load(void *p, AG_DataSource *ds, const AG_Version *ver)
{
	VG_Text *vt = p;

	if ((vt->p1 = VG_ReadRef(ds, vt, "Point")) == NULL ||
	    (vt->p2 = VG_ReadRef(ds, vt, "Point")) == NULL)
		return (-1);

	vt->align = (enum vg_alignment)AG_ReadUint8(ds);
	AG_CopyString(vt->fontFace, ds, sizeof(vt->fontFace));
	vt->fontSize = (int)AG_ReadUint8(ds);
	vt->fontFlags = (Uint)AG_ReadUint16(ds);
	AG_CopyString(vt->text, ds, sizeof(vt->text));
	vt->nPtrs = 0;
	return (0);
}

static void
Save(void *p, AG_DataSource *ds)
{
	VG_Text *vt = p;

	VG_WriteRef(ds, vt->p1);
	VG_WriteRef(ds, vt->p2);
	AG_WriteUint8(ds, (Uint8)vt->align);
	AG_WriteString(ds, vt->fontFace);
	AG_WriteUint8(ds, (Uint8)vt->fontSize);
	AG_WriteUint16(ds, (Uint16)vt->fontFlags);
	AG_WriteString(ds, vt->text);
}

/* Specify text with polled values. */
void
VG_TextPrintfPolled(VG_Text *vt, const char *fmt, ...)
{
	const char *p;
	va_list ap;
	
	VG_Lock(VGNODE(vt)->vg);
	
	if (fmt != NULL) {
		Strlcpy(vt->text, fmt, sizeof(vt->text));
	} else {
		vt->text[0] = '\0';
		goto out;
	}

	va_start(ap, fmt);
	for (p = &fmt[0]; *p != '\0'; p++) {
		if (*p == '%' && *(p+1) != '\0') {
			switch (*(p+1)) {
			case ' ':
			case '(':
			case ')':
			case '%':
				break;
			default:
				if (vt->nPtrs >= VG_TEXT_MAX_PTRS) {
					break;
				}
				vt->ptrs[vt->nPtrs++] = va_arg(ap, void *);
				break;
			}
		}
	}
	va_end(ap);
out:
	VG_Unlock(VGNODE(vt)->vg);
}

/* Specify static text. */
void
VG_TextPrintf(VG_Text *vt, const char *fmt, ...)
{
	va_list ap;

	VG_Lock(VGNODE(vt)->vg);
	
	if (fmt != NULL) {
		va_start(ap, fmt);
		Vsnprintf(vt->text, sizeof(vt->text), fmt, ap);
		va_end(ap);
	} else {
		vt->text[0] = '\0';
	}

	VG_Unlock(VGNODE(vt)->vg);
}

static void
RenderTextStatic(VG_Text *vt, const char *s, VG_View *vv)
{
	VG_Vector v1, v2, vMid;
	int x, y, w, h;
	int su = -1;			/* Make compiler happy */
	SDL_Surface *suTmp = NULL;

	AG_PushTextState();
	if (vt->fontFace[0] != '\0' &&
	   ((agGUI && vt->fontSize != agDefaultFont->size) ||
	    (agGUI && vt->fontFlags != agDefaultFont->flags))) {
		AG_TextFontLookup(vt->fontFace, vt->fontSize, vt->fontFlags);
	}
	AG_TextColorVideo32(VG_MapColorRGB(VGNODE(vt)->color));

	if (agTextCache) {
		su = AG_TextCacheInsLookup(vv->tCache, s);
		w = WSURFACE(vv,su)->w;
		h = WSURFACE(vv,su)->h;
	} else {
		suTmp = AG_TextRender(s);
		w = suTmp->w;
		h = suTmp->h;
	}

	v1 = VG_Pos(vt->p1);
	v2 = VG_Pos(vt->p2);
	vMid.x = v1.x + (v2.x - v1.x)/2.0f;
	vMid.y = v1.y + (v2.y - v1.y)/2.0f;
	VG_GetViewCoords(vv, vMid, &x, &y);
#ifdef HAVE_OPENGL
	if (agView->opengl) {
		glPushMatrix();
		glTranslatef((float)(WIDGET(vv)->rView.x1 + x),
		             (float)(WIDGET(vv)->rView.y1 + y),
			     0.0f);
		glRotatef(VG_Degrees(VG_Atan2(v1.y-v2.y, v1.x-v2.x)),
		    0.0f, 0.0f, 1.0f);
		if (agTextCache) {
			AG_WidgetBlitSurfaceGL(vv, su, w, h);
		} else {
			AG_WidgetBlitGL(vv, suTmp, w, h);
			AG_SurfaceFree(suTmp);
		}
		glPopMatrix();
	} else
#endif
	{
		if (agTextCache) {
			AG_WidgetBlitSurface(vv, su,
			    x - w/2,
			    y - h/2);
		} else {
#ifdef HAVE_OPENGL
			if (agView->opengl) {
				AG_WidgetBlitGL(vv, suTmp,
				    x - w/2,
				    y - h/2);
			} else
#endif
			{
				/* TODO */
			}
			AG_SurfaceFree(suTmp);
		}
	}
	AG_PopTextState();
}

#define TARG(_type) (*(_type *)vt->ptrs[ri])

static void
RenderTextPolled(VG_Text *vt, VG_View *vv)
{
	char s[VG_TEXT_MAX], s2[32];
	char *fmtp;
	int ri = 0;
	
	s[0] = '\0';
	for (fmtp = &vt->text[0]; *fmtp != '\0'; fmtp++) {
		if (*fmtp == '%' && *(fmtp+1) != '\0') {
			switch (*(fmtp+1)) {
			case 'd':
			case 'i':
				Snprintf(s2, sizeof(s2), "%d", TARG(int));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'o':
				Snprintf(s2, sizeof(s2), "%o", TARG(Uint));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'u':
				Snprintf(s2, sizeof(s2), "%u", TARG(Uint));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'x':
				Snprintf(s2, sizeof(s2), "%x", TARG(Uint));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'X':
				Snprintf(s2, sizeof(s2), "%X", TARG(Uint));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'c':
				s2[0] = TARG(char);
				s2[1] = '\0';
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 's':
				Strlcat(s, &TARG(char), sizeof(s));
				ri++;
				break;
			case 'f':
				Snprintf(s2, sizeof(s2), "%.2f", TARG(float));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case 'F':
				Snprintf(s2, sizeof(s2), "%.2f", TARG(double));
				Strlcat(s, s2, sizeof(s));
				ri++;
				break;
			case '%':
				s2[0] = '%';
				s2[1] = '\0';
				Strlcat(s, s2, sizeof(s));
				break;
			}
			fmtp++;
		} else {
			s2[0] = *fmtp;
			s2[1] = '\0';
			Strlcat(s, s2, sizeof(s));
		}
	}
	RenderTextStatic(vt, s, vv);
}

#undef TARG

static void
Draw(void *p, VG_View *vv)
{
	VG_Text *vt = p;
	
	if (vt->nPtrs > 0) {
		RenderTextPolled(vt, vv);
	} else {
		RenderTextStatic(vt, vt->text, vv);
	}
}

static void
Extent(void *p, VG_View *vv, VG_Vector *a, VG_Vector *b)
{
	VG_Text *vt = p;
	float wText, hText;
	VG_Vector v1, v2;
	AG_Surface *suTmp = NULL;		/* Make compiler happy */
	int su;

	if (agTextCache) {
		su = AG_TextCacheInsLookup(vv->tCache, vt->text);
		wText = (float)WSURFACE(vv,su)->w/vv->scale;
		hText = (float)WSURFACE(vv,su)->h/vv->scale;
	} else {
		suTmp = AG_TextRender(vt->text);
		wText = (float)suTmp->w/vv->scale;
		hText = (float)suTmp->h/vv->scale;
	}

	v1 = VG_Pos(vt->p1);
	v2 = VG_Pos(vt->p2);
	
	if (!agTextCache) {
		AG_SurfaceFree(suTmp);
	}

	a->x = MIN(v1.x,v2.x) - wText/2.0f;
	a->y = MIN(v1.y,v2.y) - hText/2.0f;
	b->x = MAX(v1.x,v2.x) + hText/2.0f;
	b->y = MAX(v1.y,v2.y) + hText/2.0f;
}

static float
PointProximity(void *p, VG_View *vv, VG_Vector *vPt)
{
	VG_Text *vt = p;
	VG_Vector v1 = VG_Pos(vt->p1);
	VG_Vector v2 = VG_Pos(vt->p2);

	/* XXX TODO */
	return VG_PointLineDistance(v1, v2, vPt);
}

static void
Delete(void *p)
{
	VG_Text *vt = p;

	if (VG_DelRef(vt, vt->p1) == 0)
		VG_Delete(vt->p1);
	if (VG_DelRef(vt, vt->p2) == 0)
		VG_Delete(vt->p2);
}

static void
SetAlign(AG_Event *event)
{
	VG_Text *vt = AG_PTR(1);
	enum vg_alignment align = (enum vg_alignment)AG_INT(2);

	vt->align = align;
}

static void
SelectFont(AG_Event *event)
{
	VG_Text *vt = AG_PTR(1);
	AG_Window *win = AG_PTR(2);
	AG_FontSelector *fs = AG_PTR(3);

	Strlcpy(vt->fontFace, fs->curFace, sizeof(vt->fontFace));
	vt->fontSize = fs->curSize;
	vt->fontFlags = 0;
	if (fs->curStyle & AG_FONT_BOLD) { vt->fontFlags |= VG_TEXT_BOLD; }
	if (fs->curStyle & AG_FONT_ITALIC) { vt->fontFlags |= VG_TEXT_ITALIC; }

	AG_ViewDetach(win);
}

static void
SelectFontDlg(AG_Event *event)
{
	VG_Text *vt = AG_PTR(1);
	VG_View *vv = AG_PTR(2);
	AG_Window *win, *winParent;
	AG_FontSelector *fs;
	AG_Box *hBox;

	win = AG_WindowNew(0);
	AG_WindowSetCaption(win, _("Font selection"));

	fs = AG_FontSelectorNew(win, AG_FONTSELECTOR_EXPAND);

	hBox = AG_BoxNewHoriz(win, AG_BOX_HFILL|AG_BOX_HOMOGENOUS);
	AG_ButtonNewFn(hBox, 0, _("OK"), SelectFont, "%p,%p,%p", vt, win, fs);
	AG_ButtonNewFn(hBox, 0, _("Close"), AG_WindowCloseGenEv, "%p", win);

	AG_WindowShow(win);
	if ((winParent = AG_ParentWindow(vv)) != NULL)
		AG_WindowAttach(winParent, win);
}

static void *
Edit(void *p, VG_View *vv)
{
	VG_Text *vt = p;
	AG_Box *box = AG_BoxNewVert(NULL, AG_BOX_EXPAND);
	AG_Pane *vPane;
	AG_Textbox *tb;
	AG_Box *bAl, *bAlv;

	vPane = AG_PaneNewVert(box, AG_PANE_EXPAND);

	AG_LabelNew(vPane->div[0], 0, _("Text: "));
	tb = AG_TextboxNew(vPane->div[0],
	    AG_TEXTBOX_MULTILINE|AG_TEXTBOX_EXPAND,
	    NULL);
	AG_TextboxBindUTF8(tb, vt->text, sizeof(vt->text));

	bAlv = AG_BoxNewVertNS(vPane->div[1], AG_BOX_HFILL|AG_BOX_FRAME);
	AG_LabelNew(bAlv, 0, _("Alignment: "));
	bAl = AG_BoxNewHorizNS(bAlv, AG_BOX_HFILL|AG_BOX_HOMOGENOUS);
	AG_ButtonNewFn(bAl, 0, _("TL"), SetAlign, "%p,%i", vt, VG_ALIGN_TL);
	AG_ButtonNewFn(bAl, 0, _("TC"), SetAlign, "%p,%i", vt, VG_ALIGN_TC);
	AG_ButtonNewFn(bAl, 0, _("TR"), SetAlign, "%p,%i", vt, VG_ALIGN_TR);
	bAl = AG_BoxNewHorizNS(bAlv, AG_BOX_HFILL|AG_BOX_HOMOGENOUS);
	AG_ButtonNewFn(bAl, 0, _("ML"), SetAlign, "%p,%i", vt, VG_ALIGN_ML);
	AG_ButtonNewFn(bAl, 0, _("MC"), SetAlign, "%p,%i", vt, VG_ALIGN_MC);
	AG_ButtonNewFn(bAl, 0, _("MR"), SetAlign, "%p,%i", vt, VG_ALIGN_MR);
	bAl = AG_BoxNewHorizNS(bAlv, AG_BOX_HFILL|AG_BOX_HOMOGENOUS);
	AG_ButtonNewFn(bAl, 0, _("BL"), SetAlign, "%p,%i", vt, VG_ALIGN_BL);
	AG_ButtonNewFn(bAl, 0, _("BC"), SetAlign, "%p,%i", vt, VG_ALIGN_BC);
	AG_ButtonNewFn(bAl, 0, _("BR"), SetAlign, "%p,%i", vt, VG_ALIGN_BR);

	AG_ButtonNewFn(vPane->div[1], AG_BUTTON_HFILL, _("Select font"),
	    SelectFontDlg, "%p,%p", vt, vv);
	AG_CheckboxNewFlag(vPane->div[1], 0, _("Underline"),
	    &vt->fontFlags, VG_TEXT_UNDERLINE);
	AG_CheckboxNewFlag(vPane->div[1], 0, _("Scale to view"),
	    &vt->fontFlags, VG_TEXT_SCALED);

	return (box);
}

VG_NodeOps vgTextOps = {
	N_("Text"),
	&vgIconText,
	sizeof(VG_Text),
	Init,
	NULL,			/* destroy */
	Load,
	Save,
	Draw,
	Extent,
	PointProximity,
	NULL,			/* lineProximity */
	Delete,
	NULL,			/* moveNode */
	Edit
};
