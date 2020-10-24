function
draw_pass1(F, X)
{
	var i, n, x;
	draw_array = [];
	n = DRAW_WIDTH + 1; // +1 eliminates aliasing
	for (i = 1; i < n; i++) {
		x = xmin + (xmax - xmin) * i / n;
		draw_point(F, X, x, 1);
	}
}
