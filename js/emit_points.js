function
emit_points()
{
	var i, n, x, y;

	n = draw_array.length;

	for (i = 0; i < n; i++) {

		x = draw_array[i].x;
		y = draw_array[i].y;

		if (!inrange(x, y))
			continue;

		x += DRAW_LEFT_PAD;
		y = DRAW_HEIGHT - y + DRAW_TOP_PAD;

		x = "cx='" + x.toFixed(2) + "'";
		y = "cy='" + y.toFixed(2) + "'";

		outbuf += "<circle " + x + y + "r='1.5' style='stroke:black;fill:black'/>\n";
	}
}
