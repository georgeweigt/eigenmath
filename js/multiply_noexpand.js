function
multiply_noexpand()
{
	var t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}
