function
draw_line(x1, y1, x2, y2, t)
{
	if (x1 == x2 && y1 == y2) {
		x1 -= t / 2;
		x2 += t / 2;
	}

	x1 += DRAW_INDENT;
	x2 += DRAW_INDENT;

	y1 += DRAW_PAD;
	y2 += DRAW_PAD;

	x1 = "x1='" + x1 + "'";
	y1 = "y1='" + y1 + "'";

	x2 = "x2='" + x2 + "'";
	y2 = "y2='" + y2 + "'";

	outbuf += "<line " + x1 + y1 + x2 + y2 + "style='stroke:black;stroke-width:" + t + "'/>\n";
}
