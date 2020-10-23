function
draw_data(F, X)
{
	var i, x;
	for (i = 0; i < DRAW_COUNT; i++) {
		x = xmin + (xmax - xmin) * i / DRAW_COUNT;
		draw_point(F, X, x);
	}
}
