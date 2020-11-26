#include "app.h"

double
get_descent(int font_num)
{
	double d;
	CTFontRef f;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return d;
}
