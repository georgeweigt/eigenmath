function
negate_noexpand()
{
	var t = expanding;
	expanding = 0;
	negate();
	expanding = t;
}
