/*	Public domain	*/

#ifndef _AGAR_WIDGET_MENU_H_
#define _AGAR_WIDGET_MENU_H_

#ifdef _AGAR_INTERNAL
#include <gui/widget.h>
#include <gui/window.h>
#include <gui/toolbar.h>
#else
#include <agar/gui/widget.h>
#include <agar/gui/window.h>
#include <agar/gui/toolbar.h>
#endif

#include "begin_code.h"

struct ag_menu;
struct ag_menu_view;

typedef struct ag_menu_item {
	const char *text;		/* Item label */
	int label;			/* Label surface name */
	int icon;			/* Icon name */
	int state;			/* State (for dynamic items) */
	SDLKey key_equiv;		/* Key shortcut */
	SDLMod key_mod;
	int x, y;			/* Position in parent view */
	struct ag_menu_item *subitems;	/* Child items */
	Uint nsubitems;
	AG_Event *onclick;		/* Raised on click */
	AG_Event *poll;			/* Raised before the item is drawn */
	Uint flags;
#define AG_MENU_ITEM_ICONS	0x01	/* Some of the subitems have icons */
#define AG_MENU_ITEM_NOSELECT	0x02	/* Item is non-selectable */

	enum ag_menu_binding {
		AG_MENU_NO_BINDING,
		AG_MENU_INT_BOOL,
		AG_MENU_INT8_BOOL,
		AG_MENU_INT_FLAGS,
		AG_MENU_INT8_FLAGS,
		AG_MENU_INT16_FLAGS,
		AG_MENU_INT32_FLAGS
	} bind_type;
	void *bind_p;			/* Pointer to bound variable */
	Uint32 bind_flags;		/* Bitmask to control */
	int bind_invert;		/* Invert the option */
	AG_Mutex *bind_lock;		/* Lock when accessing binding */

	struct ag_menu_view *view;	/* Back pointer to view (subitems) */
	struct ag_menu *pmenu;		/* Parent menu */
	struct ag_menu_item *pitem;	/* Parent item (NULL for top items) */
	struct ag_menu_item *sel_subitem; /* Selected subitem */
} AG_MenuItem;

typedef struct ag_menu {
	struct ag_widget wid;
	Uint flags;
#define AG_MENU_HFILL	0x01
#define AG_MENU_VFILL	0x02
#define AG_MENU_EXPAND	(AG_MENU_HFILL|AG_MENU_VFILL)
	AG_MenuItem *items;		/* Top-level items */
	Uint nitems;
	int selecting;			/* Selection in progress */
	AG_MenuItem *sel_item;		/* Selected top-level item */
	int hspace, vspace;		/* Spacing */
	int itemh;			/* Item height (optimization) */
} AG_Menu;

typedef struct ag_popup_menu {
	AG_Menu *menu;
	AG_MenuItem *item;
	AG_Window *win;
	SLIST_ENTRY(ag_popup_menu) menus;
} AG_PopupMenu;

typedef struct ag_menu_view {
	struct ag_widget wid;
	AG_Window *panel;
	AG_Menu *pmenu;
	AG_MenuItem *pitem;
	int hspace, vpadding;
	AG_Timeout submenu_to;
} AG_MenuView;

__BEGIN_DECLS
AG_Menu	  *AG_MenuNew(void *, Uint);
void	   AG_MenuInit(AG_Menu *, Uint);
void 	   AG_MenuScale(void *, int, int);
void	   AG_MenuDraw(void *);
void 	   AG_MenuDestroy(void *);

AG_PopupMenu	*AG_PopupNew(void *);
__inline__ void	 AG_PopupShow(AG_PopupMenu *);
__inline__ void	 AG_PopupShowAt(AG_PopupMenu *, int, int);
__inline__ void	 AG_PopupHide(AG_PopupMenu *);
void		 AG_PopupDestroy(AG_PopupMenu *);

AG_MenuItem *AG_MenuAddItem(AG_Menu *, const char *);
void	     AG_MenuFreeItems(AG_Menu *);
void	     AG_MenuFreeSubItems(AG_MenuItem *);
AG_Window   *AG_MenuExpand(AG_Menu *, AG_MenuItem *, int, int);
void   	     AG_MenuCollapse(AG_Menu *, AG_MenuItem *);

__inline__ void AG_MenuSetIcon(AG_MenuItem *, SDL_Surface *);
__inline__ void AG_MenuSetLabel(AG_MenuItem *, const char *);

AG_MenuItem *AG_MenuAction(AG_MenuItem *, const char *, int,
			   void (*)(AG_Event *), const char *, ...);
AG_MenuItem *AG_MenuActionKb(AG_MenuItem *, const char *, int, SDLKey, SDLMod,
	                     void (*)(AG_Event *), const char *, ...);
AG_MenuItem *AG_MenuTool(AG_MenuItem *, AG_Toolbar *, const char *, int,
			 SDLKey, SDLMod, void (*)(AG_Event *),
			 const char *, ...);
AG_MenuItem *AG_MenuDynamic(AG_MenuItem *, int, void (*)(AG_Event *),
			    const char *, ...);
AG_MenuItem *AG_MenuIntBoolMp(AG_MenuItem *, const char *, int, int *, int,
			      AG_Mutex *);
AG_MenuItem *AG_MenuInt8BoolMp(AG_MenuItem *, const char *, int, Uint8 *, int,
			       AG_Mutex *);
AG_MenuItem *AG_MenuIntFlagsMp(AG_MenuItem *, const char *, int, int *, int,
			       int, AG_Mutex *);
AG_MenuItem *AG_MenuInt8FlagsMp(AG_MenuItem *, const char *, int, Uint8 *,
				Uint8, int, AG_Mutex *);
AG_MenuItem *AG_MenuInt16FlagsMp(AG_MenuItem *, const char *, int, Uint16 *,
				 Uint16, int, AG_Mutex *);
AG_MenuItem *AG_MenuInt32FlagsMp(AG_MenuItem *, const char *, int, Uint32 *,
				 Uint32, int, AG_Mutex *);

#define AG_MenuNode(mi,name,icon) AG_MenuAction((mi),(name),(icon),NULL,NULL)

#define AG_MenuIntBool(mi,t,i,p,inv) \
	AG_MenuIntBoolMp((mi),(t),(i),(p),(inv),NULL)
#define AG_MenuInt8Bool(mi,t,i,p,inv) \
	AG_MenuInt8BoolMp((mi),(t),(i),(p),(inv),NULL)

#define AG_MenuIntFlags(mi,t,i,fp,fl,inv) \
	AG_MenuIntFlagsMp((mi),(t),(i),(fp),(fl),(inv),NULL)
#define AG_MenuInt8Flags(mi,t,i,fp,fl,inv) \
	AG_MenuInt8FlagsMp((mi),(t),(i),(fp),(fl),(inv),NULL)
#define AG_MenuInt16Flags(mi,t,i,fp,fl,inv) \
	AG_MenuInt16FlagsMp((mi),(t),(i),(fp),(fl),(inv),NULL)
#define AG_MenuInt32Flags(mi,t,i,fp,fl,inv) \
	AG_MenuInt32FlagsMp((mi),(t),(i),(fp),(fl),(inv),NULL)

AG_MenuItem *AG_MenuSeparator(AG_MenuItem *);
AG_MenuItem *AG_MenuSection(AG_MenuItem *, const char *, ...);

void AG_MenuViewInit(void *, AG_Window *, AG_Menu *, AG_MenuItem *);
void AG_MenuViewDraw(void *);
void AG_MenuViewScale(void *, int, int);
__END_DECLS

#include "close_code.h"
#endif /* _AGAR_WIDGET_MENU_H_ */
