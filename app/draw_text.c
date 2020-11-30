#include "app.h"

void
draw_text(double x, double y, uint8_t *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		draw_char(x, y, TEXT_FONT, buf[i]);
		x += get_char_width(TEXT_FONT, buf[i]);
	}
}
