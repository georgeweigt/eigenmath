function
multiply_expand()
{
	var t;
	t = expanding;
	expanding = 1;
	multiply();
	expanding = t;
}
