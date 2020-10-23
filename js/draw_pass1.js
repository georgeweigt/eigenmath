function
draw_pass1(F, X)
{
	var i, x;
	for (i = 0; i <= DRAW_WIDTH + 1; i++) {
		x = xmin + (xmax - xmin) * i / (DRAW_WIDTH + 1); // +1 eliminates aliasing
		draw_point(F, X, x, 1);
	}
}
