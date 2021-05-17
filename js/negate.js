function
negate()
{
	push_integer(-1);
	multiply();
}

function
negate_expand()
{
	var t = expanding;
	expanding = 1;
	negate();
	expanding = t;
}
