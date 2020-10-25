function
emit_svg_text(p, x, y)
{
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

	t = "<text style='text-anchor:middle;font-family:\"Times New Roman\";";

	if (p.small_font)
		t += "font-size:14pt;";
	else
		t += "font-size:20pt;";

	if (p.italic_font)
		t += "font-style:italic;";

	t += "'" + x + y + ">" + s + "</text>\n";

	outbuf += t;
}
