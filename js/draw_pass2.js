function
draw_pass2(F, X)
{
	var i, j, m, n, p1, p2, x;

	n = draw_array.length;

	for (i = 0; i < n - 1; i++) {

		p1 = draw_array[i];
		p2 = draw_array[i + 1];

		if ((p1.dy >= 0 && p1.dy <= DRAW_HEIGHT) || (p2.dy >= 0 && p2.dy <= DRAW_HEIGHT)) {

			m = Math.floor(Math.abs(p1.dy - p2.dy));

			for (j = 1; j < m; j++) {
				x = p1.x + (p2.x - p1.x) * j / m;
				draw_eval(F, X, x);
			}
		}
	}
}
