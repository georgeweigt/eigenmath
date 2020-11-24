#include "app.h"

CTFontRef
get_font_ref(int font_num)
{
	return font_ref_tab[font_num];
}

int
get_ascent(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetAscent(f);
	return ceil(h);
}

double
get_char_height(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetCapHeight(f);
	return h;
}

double
get_underline_position(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetUnderlinePosition(f);
	return -h;
}

int
get_descent(int font_num)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return ceil(d);
}

int roman_tab[256] = {

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

int italic_tab[256] = {

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
	int k;
	CTFontRef f;
	CGFloat d;

	if (font_num == ITALIC_FONT || font_num == SMALL_ITALIC_FONT)
		k = italic_tab[char_num];
	else
		k = roman_tab[char_num];

	if (k == 0)
		return 0.0;

	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);

	return d;
}

double
get_xheight(int font_num)
{
	CTFontRef f;
	double h;
	f = get_font_ref(font_num);
	h = CTFontGetXHeight(f);
	return h;
}

int
get_width(int font_num, int char_num)
{
	CTFontRef f;
	CFStringRef name;
	CGGlyph g;
	double w;

	f = get_font_ref(font_num);

	name = get_char_name(char_num);

	g = CTFontGetGlyphWithName(f, name);
	w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);

	return ceil(w);
}

double
get_char_width(int font_num, int char_num)
{
	CTFontRef f;
	CFStringRef name;
	CGGlyph g;
	double w;

	f = get_font_ref(font_num);
	name = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, name);
	w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);

	return ceil(w);
}

int
text_width(int font_num, char *s)
{
	int w = 0;
	while (*s)
		w += get_width(font_num, *s++);
	return w;
}

void
get_height_width(int *h, int *w, int font_num, char *s)
{
	*h = get_ascent(font_num) + get_descent(font_num);
	*w = text_width(font_num, s);
}

// Draw a little square at coordinates x + dx, y + dy

void
draw_point(int x, int dx, int y, int dy)
{
	float fx1, fx2, fy1, fy2;

	fx1 = x + dx - 0.5;
	fx2 = x + dx + 0.5;

	fy1 = app_total_h - (y + dy - 0.5);
	fy2 = app_total_h - (y + dy + 0.5);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

//	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 1.0, 1.0);
	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
//	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);
}

void
draw_box(int x1, int y1, int x2, int y2)
{
	float fx1, fx2, fy1, fy2;

	fx1 = x1 - 0.5;
	fx2 = x2 + 0.5;

	fy1 = app_total_h - (y1 - 0.5);
	fy2 = app_total_h - (y2 + 0.5);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
}
