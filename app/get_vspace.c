#include "app.h"

double
get_vspace(int font_num)
{
	CTFontRef f;
	double h;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return 0.25 * h;
}
