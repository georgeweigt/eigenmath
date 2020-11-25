#include "app.h"

double
get_char_height(int font_num)
{
	CTFontRef f;
	double h;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return h;
}
