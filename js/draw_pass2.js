function
draw_pass2(F, X)
{
	var i, j, m, n, p1, p2, x;

	n = draw_array.length;

	if (n < 2)
		return;

	for (i = 0; i < n - 1; i++) {

		p1 = draw_array[i];
		p2 = draw_array[i + 1];

		if (p1.drawn == 0 && p2.drawn == 0)
			continue;

		m = Math.floor(Math.abs(p1.y - p2.y));

		for (j = 1; j < m; j++) {
			x = p1.x + (p2.x - p1.x) * j / m;
			draw_point(F, X, x, 0);
		}
	}
}
