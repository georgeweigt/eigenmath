function
emit_points()
{
	var i, n, x, y;

	n = draw_buf.length;

	for (i = 0; i < n; i++) {

		x = draw_buf[i].x;
		y = draw_buf[i].y;

		if (!inrange(x, y))
			continue;

		x += DRAW_LEFT_PAD;
		y = DRAW_HEIGHT - y + DRAW_TOP_PAD;

		x = "cx='" + x + "'";
		y = "cy='" + y + "'";

		outbuf += "<circle " + x + y + "r='1.5' style='stroke:black;fill:black'/>\n";
	}
}
