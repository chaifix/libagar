/*	$Csoft: icons.h,v 1.2 2004/04/17 00:31:13 vedge Exp $	*/
/*	Public domain	*/

#ifndef _AGAR_ICONS_H_
#define _AGAR_ICONS_H_
#include "begin_code.h"

enum {
	OBJECT_EDITOR_ICON,
	STAMP_TOOL_ICON,
	ERASER_TOOL_ICON,
	NEW_VIEW_ICON,
	MAGNIFIER_TOOL_ICON,
	RESIZE_TOOL_ICON,
	GRID_ICON,
	PROPS_ICON,
	PROPEDIT_ICON,
	EDIT_ICON,
	RIGHT_ARROW_ICON,
	LEFT_ARROW_ICON,
	UP_ARROW_ICON,
	DOWN_ARROW_ICON,
	NODE_EDITOR_ICON,
	LAYER_EDITOR_ICON,
	SELECT_TOOL_ICON,
	SHIFT_TOOL_ICON,
	MERGE_TOOL_ICON,
	FILL_TOOL_ICON,
	FLIP_TOOL_ICON,
	MEDIASEL_ICON,
	POSITION_TOOL_ICON,
	INVERT_TOOL_ICON,
	SETTINGS_ICON,

	SELECT_CURSOR,
	FILL_CURSOR,
	MAGNIFIER_CURSOR,
	VERTRESIZE_CURSOR,
	HORIZRESIZE_CURSOR,
	ERASER_CURSOR,

	OBJCREATE_ICON,
	OBJEDIT_ICON,
	OBJGENEDIT_ICON,
	OBJLOAD_ICON,
	OBJSAVE_ICON,
	OBJDUP_ICON,
	OBJMOVEUP_ICON,
	OBJMOVEDOWN_ICON,
	TRASH_ICON,

	OBJ_ICON,
	MAP_ICON,
	VGOBJ_ICON,
	PERSO_ICON,

	VGORIGIN_ICON,
	VGPOINTS_ICON,
	VGLINES_ICON,
	VGTRIANGLES_ICON,
	VGCIRCLES_ICON,
	VGBEZIER_CURVE_ICON,
	VGRGB_ICON,

	SNAP_FREE_ICON,
	SNAP_RINT_ICON,
	SNAP_GRID_ICON,
	SNAP_ENDPOINT_ICON,
	SNAP_ENDPOINT_D_ICON,
	SNAP_CLOSEST_ICON,
	SNAP_CENTERPT_ICON,
	SNAP_MIDDLEPT_ICON,
	SNAP_INTSECT_AUTO_ICON,
	SNAP_INTSECT_MANUAL_ICON
};

extern struct object engine_icons;			/* engine.c */
#define ICON(n)	SPRITE(&engine_icons, (n))

#include "close_code.h"
#endif	/* _AGAR_ICONS_H_ */
