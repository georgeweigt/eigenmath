function
emit_svg_text(s, small_font, italic_font, x, y)
{
	outbuf += "<text style='text-anchor:middle;font-family:times;";

	if (small_font) {
		if (italic_font)
			outbuf += "font-size:14pt;font-style:italic;'";
		else
			outbuf += "font-size:14pt;'";
	} else {
		if (italic_font)
			outbuf += "font-size:20pt;font-style:italic;'";
		else
			outbuf += "font-size:20pt;'";
	}

	outbuf += " x='" + x + "' y='" + y + "'>" + s + "</text>";
}
