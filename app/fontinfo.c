#include "app.h"

CTFontRef font_ref_tab[5];

void
init_fonts(void)
{
	font_ref_tab[TEXT_FONT] = CTFontCreateWithName(CFSTR("Courier"), SMALL_FONT_SIZE, NULL);

	font_ref_tab[ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), FONT_SIZE, NULL);
	font_ref_tab[ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), FONT_SIZE, NULL);

	font_ref_tab[SMALL_ROMAN_FONT] = CTFontCreateWithName(CFSTR("Times New Roman"), SMALL_FONT_SIZE, NULL);
	font_ref_tab[SMALL_ITALIC_FONT] = CTFontCreateWithName(CFSTR("Times New Roman Italic"), SMALL_FONT_SIZE, NULL);
}

double
get_ascent(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetAscent(f);
	return h;
}

double
get_cap_height(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return h;
}

int roman_descent_tab[256] = {

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	  ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
	0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [   ] ^ _
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,

//	` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
	0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // upper case greek
	0,1,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,1,0, // lower case greek

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int italic_descent_tab[256] = {

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	  ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
	0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

//	@ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [   ] ^ _
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,

//	` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
	0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // upper case greek
	0,1,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,1,0, // lower case greek

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

double
get_char_depth(int font_num, int char_num)
{
	int t;
	double d;

	if (font_num == ITALIC_FONT || font_num == SMALL_ITALIC_FONT)
		t = italic_descent_tab[char_num];
	else
		t = roman_descent_tab[char_num];

	if (t)
		d = get_descent(font_num);
	else
		d = get_leading(font_num);

	return d;
}

#define NA CFSTR("question")

CFStringRef char_name_tab[256] = {

	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,

	CFSTR("space"),	// 32
	CFSTR("exclam"),
	CFSTR("quotedbl"),
	CFSTR("numbersign"),
	CFSTR("dollar"),
	CFSTR("percent"),
	CFSTR("ampersand"),
	CFSTR("quotesingle"),
	CFSTR("parenleft"),
	CFSTR("parenright"),
	CFSTR("asterisk"),
	CFSTR("plus"),
	CFSTR("comma"),
	CFSTR("hyphen"),
	CFSTR("period"),
	CFSTR("slash"),
	CFSTR("zero"),
	CFSTR("one"),
	CFSTR("two"),
	CFSTR("three"),
	CFSTR("four"),
	CFSTR("five"),
	CFSTR("six"),
	CFSTR("seven"),
	CFSTR("eight"),
	CFSTR("nine"),
	CFSTR("colon"),
	CFSTR("semicolon"),
	CFSTR("less"),
	CFSTR("equal"),
	CFSTR("greater"),
	CFSTR("question"),
	CFSTR("at"),
	CFSTR("A"),
	CFSTR("B"),
	CFSTR("C"),
	CFSTR("D"),
	CFSTR("E"),
	CFSTR("F"),
	CFSTR("G"),
	CFSTR("H"),
	CFSTR("I"),
	CFSTR("J"),
	CFSTR("K"),
	CFSTR("L"),
	CFSTR("M"),
	CFSTR("N"),
	CFSTR("O"),
	CFSTR("P"),
	CFSTR("Q"),
	CFSTR("R"),
	CFSTR("S"),
	CFSTR("T"),
	CFSTR("U"),
	CFSTR("V"),
	CFSTR("W"),
	CFSTR("X"),
	CFSTR("Y"),
	CFSTR("Z"),
	CFSTR("bracketleft"),
	CFSTR("backslash"),
	CFSTR("bracketright"),
	CFSTR("asciicircum"),
	CFSTR("underscore"),
	CFSTR("grave"),
	CFSTR("a"),
	CFSTR("b"),
	CFSTR("c"),
	CFSTR("d"),
	CFSTR("e"),
	CFSTR("f"),
	CFSTR("g"),
	CFSTR("h"),
	CFSTR("i"),
	CFSTR("j"),
	CFSTR("k"),
	CFSTR("l"),
	CFSTR("m"),
	CFSTR("n"),
	CFSTR("o"),
	CFSTR("p"),
	CFSTR("q"),
	CFSTR("r"),
	CFSTR("s"),
	CFSTR("t"),
	CFSTR("u"),
	CFSTR("v"),
	CFSTR("w"),
	CFSTR("x"),
	CFSTR("y"),
	CFSTR("z"),
	CFSTR("braceleft"),
	CFSTR("bar"),
	CFSTR("braceright"),
	CFSTR("asciitilde"),
	CFSTR("controlDEL"),

	CFSTR("Alpha"),	// 128
	CFSTR("Beta"),
	CFSTR("Gamma"),
	CFSTR("Delta"),
	CFSTR("Epsilon"),
	CFSTR("Zeta"),
	CFSTR("Eta"),
	CFSTR("Theta"),
	CFSTR("Iota"),
	CFSTR("Kappa"),
	CFSTR("Lambda"),
	CFSTR("Mu"),
	CFSTR("Nu"),
	CFSTR("Xi"),
	CFSTR("Omicron"),
	CFSTR("Pi"),
	CFSTR("Rho"),
	CFSTR("Sigma"),
	CFSTR("Tau"),
	CFSTR("Upsilon"),
	CFSTR("Phi"),
	CFSTR("Chi"),
	CFSTR("Psi"),
	CFSTR("Omega"),

	CFSTR("alpha"),	// 152
	CFSTR("beta"),
	CFSTR("gamma"),
	CFSTR("delta"),
	CFSTR("epsilon"),
	CFSTR("zeta"),
	CFSTR("eta"),
	CFSTR("theta"),
	CFSTR("iota"),
	CFSTR("kappa"),
	CFSTR("lambda"),
	CFSTR("mu"),
	CFSTR("nu"),
	CFSTR("xi"),
	CFSTR("omicron"),
	CFSTR("pi"),
	CFSTR("rho"),
	CFSTR("sigma"),
	CFSTR("tau"),
	CFSTR("upsilon"),
	CFSTR("phi"),
	CFSTR("chi"),
	CFSTR("psi"),
	CFSTR("omega"),

	CFSTR("hbar"),		// 176 HBAR
	CFSTR("plus"),		// 177 PLUS_SIGN
	CFSTR("minus"),		// 178 MINUS_SIGN
	CFSTR("multiply"),	// 179 MULTIPLY_SIGN
	CFSTR("greaterequal"),	// 180 GREATEREQUAL
	CFSTR("lessequal"),	// 181 LESSEQUAL
	CFSTR("partialdiff"),	// 182 PARTIALDIFF
	NA,			// 183

	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
	NA, NA, NA, NA, NA, NA, NA, NA,
};

CFStringRef
get_char_name(int char_num)
{
	return char_name_tab[char_num & 0xff];
}

double
get_char_width(int font_num, int char_num)
{
	double w;
	CTFontRef f;
	CFStringRef s;
	CGGlyph g;

	f = get_font_ref(font_num);
	s = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, s);

	w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);

	return w;
}

double
get_descent(int font_num)
{
	double d;
	CTFontRef f;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return d;
}

CTFontRef
get_font_ref(int font_num)
{
	return font_ref_tab[font_num];
}

double
get_leading(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetLeading(f);
	return h;
}

double
get_xheight(int font_num)
{
	double h;
	CTFontRef f;
	f = get_font_ref(font_num);
	h = CTFontGetXHeight(f);
	return h;
}

double
get_operator_height(int font_num)
{
	return get_cap_height(font_num) / 2.0;
}
