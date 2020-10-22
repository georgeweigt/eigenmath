function
emit_svg_text(p, x, y)
{
	var s = p.s;

	if (s == '&')
		s = "&amp;";
	else if (s == '<')
		s = "&lt;";
	else if (s == '>')
		s = "&gt;";

	var X = "x='" + x + "'";
	var Y = "y='" + y + "'";

	outbuf += "<text style='text-anchor:middle;font-family:times;";

	if (p.small_font)
		outbuf += "font-size:14pt;";
	else
		outbuf += "font-size:20pt;";

	if (p.italic_font)
		outbuf += "font-style:italic;";

	outbuf += "'" + X + Y + s + "</text>";
}
