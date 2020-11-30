#include "app.h"

void
init_fonts(void)
{
	font_ref_tab[TEXT_FONT] = CTFontCreateWithName(CFSTR("Courier"), SMALL_FONT_SIZE, NULL);

	font_ref_tab[ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), FONT_SIZE, NULL);
	font_ref_tab[ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), FONT_SIZE, NULL);

	font_ref_tab[SMALL_ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), SMALL_FONT_SIZE, NULL);
	font_ref_tab[SMALL_ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), SMALL_FONT_SIZE, NULL);
}
