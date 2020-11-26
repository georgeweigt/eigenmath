#include "app.h"

double
get_ascent(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetAscent(f);
	return h;
}
