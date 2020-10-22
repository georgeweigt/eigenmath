function
emit_svg_text(p, x, y)
{
//if (p.s != " ") emit_svg_line(x + p.width, y + p.depth, x + p.width, y - p.height, 1); // for checking char widths

	var s, t;

	s = p.s;

	if (s == '&')
		s = "&amp;";
	else if (s == '<')
		s = "&lt;";
	else if (s == '>')
		s = "&gt;";

	x += p.width / 2;

	x = "x='" + x + "'";
	y = "y='" + y + "'";

	t = "<text style='text-anchor:middle;font-family:times;";

	if (p.small_font)
		t += "font-size:14pt;";
	else
		t += "font-size:20pt;";

	if (p.italic_font)
		t += "font-style:italic;";

	t += "'" + x + y + ">" + s + "</text>\n";

	outbuf += t;
}
