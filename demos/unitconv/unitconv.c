/*	Public domain	*/
/*
 * This is a simple unit converter based on AG_Units(3).
 */

#include <agar/core.h>
#include <agar/gui.h>

#include <string.h>

double value = 0.0;
const AG_Unit *unitGroup = agLengthUnits;
AG_Numerical *n1, *n2;

static void
SelectCategory(AG_Event *event)
{
	AG_Unit *group = AG_PTR(1), *unit;
		
	for (unit = &group[0]; unit->key != NULL; unit++) {
		if (unit->divider == 1) {
			AG_NumericalSetUnitSystem(n1, unit->key);
			AG_NumericalSetUnitSystem(n2, unit->key);
			return;
		}
	}
}

static void
CreateUI(void)
{
	const struct {
		const char *name;
		const AG_Unit *p;
	} units[] = {
		{ "Len", agLengthUnits },
		{ "Ang", agAngleUnits },
		{ "Mass", agMassUnits },
		{ "Area", agAreaUnits },
		{ "Vol", agVolumeUnits },
		{ "Spd", agSpeedUnits },
		{ "Time", agTimeUnits },
		{ "Temp", agTemperatureUnits },
		{ "Pwr", agPowerUnits },
		{ "Press", agPressureUnits },
		{ "Vac", agVacuumUnits },
		{ "Met", agMetabolicExpenditureUnits },
	};
	int i;
	AG_Window *win;
	AG_Toolbar *tb;

	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowSetCaption(win, "Unit Converter");
	AG_WindowSetPadding(win, 10, 10, 10, 10);
	agColors[WINDOW_BG_COLOR] = agColors[BG_COLOR];

	tb = AG_ToolbarNew(win, AG_TOOLBAR_HORIZ, 2, AG_TOOLBAR_HOMOGENOUS|
	                                             AG_TOOLBAR_STICKY);
	for (i = 0; i < sizeof(units)/sizeof(units[0]); i++) {
		if (i == 5) {
			AG_ToolbarRow(tb, 1);
		}
		AG_ToolbarButton(tb, units[i].name, (i == 0),
		    SelectCategory, "%p", units[i].p);
	}

	AG_SeparatorNewHoriz(win);

	n1 = AG_NumericalNew(win, AG_NUMERICAL_HFILL, "in", "Value: ");
	n2 = AG_NumericalNew(win, AG_NUMERICAL_HFILL, "mm", "Value: ");
	AG_WidgetBindDouble(n1, "value", &value);
	AG_WidgetBindDouble(n2, "value", &value);
	AG_NumericalSizeHint(n1, "0000.00");
	AG_NumericalSizeHint(n2, "0000.00");
	AG_NumericalSetPrecision(n1, "g", 6);
	AG_NumericalSetPrecision(n2, "g", 6);

	AG_WindowShow(win);
	AG_WindowMaximize(win);
}

int
main(int argc, char *argv[])
{
	if (AG_InitCore("unitconv", 0) == -1) {
		fprintf(stderr, "%s\n", AG_GetError());
		return (1);
	}
	AG_TextParseFontSpec("_agFontVera:14");
	if (AG_InitVideo(400, 180, 32, AG_VIDEO_RESIZABLE) == -1) {
		fprintf(stderr, "%s\n", AG_GetError());
		return (-1);
	}
	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);
	AG_BindGlobalKey(SDLK_F8, KMOD_NONE, AG_ViewCapture);
	
	CreateUI();
	agColors[WINDOW_BG_COLOR] = AG_MapRGB(agVideoFmt, 60,60,60);

	AG_EventLoop();
	AG_Destroy();
	return (0);
}

