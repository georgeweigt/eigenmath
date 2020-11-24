#include "app.h"

void
init_fonts(void)
{
	font_ref_tab[DEFAULT_FONT] = CTFontCreateWithName(CFSTR("Courier"), 16.0, NULL);

	font_ref_tab[ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), 20.0, NULL);
	font_ref_tab[ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 20.0, NULL);

	font_ref_tab[SMALL_ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), 16.0, NULL);
	font_ref_tab[SMALL_ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 16.0, NULL);
}
