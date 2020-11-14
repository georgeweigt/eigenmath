function
emit_roman_glyph(u, s)
{
	var n, v, w;

	v = {type:GLYPH, s:s, level:u.level, italic_font:0};

	switch (s) {
	case "g":
	case "j":
	case "p":
	case "q":
	case "y":
	case "(":
	case ")":
	case "[":
	case "]":
	case "{":
	case "}":
	case "@":
	case "|":
	case "_":
		if (v.level == 0)
			v.depth = FONT_DEPTH;
		else
			v.depth = SMALL_FONT_DEPTH;
		break;
	default:
		v.depth = 0;
		break;
	}

	n = s.charCodeAt(0);

	if (n < 128)
		w = roman_width[n];
	else
		w = 1000;

	if (v.level == 0) {
		v.height = FONT_HEIGHT;
		v.width = w * WIDTH_RATIO * FONT_SIZE;
	} else {
		v.height = SMALL_FONT_HEIGHT;
		v.width = w * WIDTH_RATIO * SMALL_FONT_SIZE;
	}

	u.a.push(v);
}
