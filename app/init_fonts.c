#include "app.h"

void
init_fonts(void)
{
	font_ref[DEFAULT_FONT] = CTFontCreateWithName(CFSTR("Courier"), 16.0, NULL);

	font_ref[ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), 20.0, NULL);
	font_ref[ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 20.0, NULL);

	font_ref[SMALL_ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), 16.0, NULL);
	font_ref[SMALL_ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 16.0, NULL);
}
