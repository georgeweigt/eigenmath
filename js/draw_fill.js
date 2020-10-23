function
draw_fill(F, X)
{
	var i, j, m, n, x, min, max;

	n = draw_array.length;

	if (n == 0)
		return;

	m = Math.floor(400 / n); // fill count

	min = draw_array[0];
	max = draw_array[n - 1];

	x = min - (xmax - xmin) / DRAW_COUNT;
	draw_array.unshift(x);

	x = max + (xmax - xmin) / DRAW_COUNT;
	draw_array.push(x);

	for (i = 0; i < n + 1; i++) {
		min = draw_array[i];
		max = draw_array[i + 1];
		for (j = 1; j < m; j++) {
			x = min + (max - min) * j / m;
			draw_point(F, X, x);
		}
	}
}
