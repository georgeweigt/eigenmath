function
draw_line(x1, y1, x2, y2, t)
{
	x1 += DRAW_LEFT_PAD;
	x2 += DRAW_LEFT_PAD;

	y1 += DRAW_TOP_PAD;
	y2 += DRAW_TOP_PAD;

	x1 = "x1='" + x1 + "'";
	x2 = "x2='" + x2 + "'";

	y1 = "y1='" + y1 + "'";
	y2 = "y2='" + y2 + "'";

	outbuf += "<line " + x1 + y1 + x2 + y2 + "style='stroke:black;stroke-width:" + t + "'/>\n";
}
