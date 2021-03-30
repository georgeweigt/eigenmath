function
draw_pass2(F, T)
{
	var dt, dx, dy, i, j, m, n, t, t1, t2, x1, x2, y1, y2;

	n = draw_array.length - 1;

	for (i = 0; i < n; i++) {

		t1 = draw_array[i].t;
		t2 = draw_array[i + 1].t;

		x1 = draw_array[i].x;
		x2 = draw_array[i + 1].x;

		y1 = draw_array[i].y;
		y2 = draw_array[i + 1].y;

		if (!inrange(x1, y1) && !inrange(x2, y2))
			continue;

		dt = t2 - t1;
		dx = x2 - x1;
		dy = y2 - y1;

		m = 0.5 * Math.sqrt(dx * dx + dy * dy);

		m = Math.floor(m);

		for (j = 1; j < m; j++) {
			t = t1 + dt * j / m;
			sample(F, T, t);
		}
	}
}
