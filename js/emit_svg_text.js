function
emit_svg_text(u, x, y)
{
	var s, t;

	s = u.s;

	if (s == '&')
		s = "&amp;";
	else if (s == '<')
		s = "&lt;";
	else if (s == '>')
		s = "&gt;";

	x = "x='" + x + "'";
	y = "y='" + y + "'";

	t = "<text style='font-family:\"Times New Roman\";";

	if (u.level == 0)
		t += "font-size:" + FONT_SIZE + "pt;";
	else
		t += "font-size:" + SMALL_FONT_SIZE + "pt;";

	if (u.italic_font)
		t += "font-style:italic;";

	t += "'" + x + y + ">" + s + "</text>\n";

	outbuf += t;
}
