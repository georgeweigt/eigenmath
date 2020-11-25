#include "app.h"

double
get_operator_height(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return 0.5 * h;
}
