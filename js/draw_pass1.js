function
draw_pass1(F, X)
{
	var i, n, x;
	n = DRAW_WIDTH + 1; // +1 eliminates aliasing
	for (i = 0; i <= n; i++) {
		x = xmin + (xmax - xmin) * i / n;
		draw_eval(F, X, x);
	}
}
