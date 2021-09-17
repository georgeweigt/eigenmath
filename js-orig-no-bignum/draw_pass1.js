function
draw_pass1(F, T)
{
	var i, t;
	for (i = 0; i <= DRAW_WIDTH; i++) {
		t = tmin + (tmax - tmin) * i / DRAW_WIDTH;
		sample(F, T, t);
	}
}
