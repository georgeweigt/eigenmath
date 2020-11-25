function
emit_svg_line(x1, y1, x2, y2, t)
{
	var s;

	x1 = "x1='" + x1 + "'";
	x2 = "x2='" + x2 + "'";

	y1 = "y1='" + y1 + "'";
	y2 = "y2='" + y2 + "'";

	s = "<line " + x1 + y1 + x2 + y2 + "style='stroke:black;stroke-width:" + t + "'/>\n"

	outbuf += s;
}
