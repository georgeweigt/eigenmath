#include "app.h"

double
get_xheight(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetXHeight(f);
	return h;
}
