#include "app.h"

double
get_cap_height(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return h;
}
