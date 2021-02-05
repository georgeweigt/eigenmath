function
draw_pass2(F, T)
{
	var dx, dy, i, j, m, n, t, t1, t2, x1, x2, y1, y2;

	n = draw_array.length - 1;

	for (i = 0; i < n; i++) {

		t1 = draw_array[i].t;
		t2 = draw_array[i + 1].t;

		x1 = draw_array[i].x;
		x2 = draw_array[i + 1].x;

		y1 = draw_array[i].y;
		y2 = draw_array[i + 1].y;

		if ((x1 < 0 || x1 > DRAW_WIDTH || y1 < 0 || y1 > DRAW_HEIGHT) && (x2 < 0 || x2 > DRAW_WIDTH || y2 < 0 || y2 > DRAW_HEIGHT))
			continue; // both coordinates are out of range

		dx = x2 - x1;
		dy = y2 - y1;

		m = Math.sqrt(dx * dx + dy * dy);

		m = Math.floor(m);

		for (j = 1; j < m; j++) {
			t = t1 + j / m * (t2 - t1);
			sample(F, T, t);
		}
	}
}
