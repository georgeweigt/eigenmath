function
multiply_factors_noexpand(n)
{
	var t = expanding;
	expanding = 0;
	multiply_factors(n);
	expanding = t;
}
