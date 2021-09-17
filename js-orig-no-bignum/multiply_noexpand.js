function
multiply_noexpand()
{
	var t;
	t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}
