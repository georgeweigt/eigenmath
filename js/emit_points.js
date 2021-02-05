function
emit_points()
{
	var i, n, x, y;

	n = draw_array.length;

	for (i = 0; i < n; i++) {

		x = draw_array[i].x;
		y = draw_array[i].y;

		if (x < 0 || x > DRAW_WIDTH || y < 0 || y > DRAW_HEIGHT)
			continue;

		x += DRAW_LEFT_PAD;
		y = DRAW_HEIGHT - y + DRAW_TOP_PAD;

		x = "cx='" + x + "'";
		y = "cy='" + y + "'";

		outbuf += "<circle " + x + y + "r='2' style='stroke:black;fill:black'/>";
	}
}
