#include "app.h"

double
get_leading(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetLeading(f);
	return h;
}
