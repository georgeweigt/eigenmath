function
emit_update_text(u)
{
	var descender = 0, n, w;

	switch (u.s) {
	case "f":
		if (u.italic_font)
			descender = 1;
		break;
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
		descender = 1;
		break;
	}

	n = u.s.charCodeAt(0);

	if (u.italic_font)
		w = italic_width[n];
	else
		w = roman_width[n];

	if (w == undefined)
		w = 1000;

	if (u.small_font) {
		u.height = SMALL_FONT_HEIGHT;
		if (descender)
			u.depth = SMALL_FONT_DEPTH;
		else
			u.depth = 0;
		u.width = w * WIDTH_RATIO * SMALL_FONT_SIZE;
	} else {
		u.height = FONT_HEIGHT;
		if (descender)
			u.depth = FONT_DEPTH;
		else
			u.depth = 0;
		u.width = w * WIDTH_RATIO * FONT_SIZE;
	}
}
