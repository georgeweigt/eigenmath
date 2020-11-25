#include "app.h"

double
get_vspace(int font_num)
{
	CTFontRef f;
	double d;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return d;
}
