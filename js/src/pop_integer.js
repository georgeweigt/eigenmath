function
pop_integer()
{
	var d;
	d = pop_double();
	d = Math.round(d);
	if (!Number.isFinite(d) || Math.abs(d) > 0x7fffffff)
		stopf("integer overflow");
	return d;
}
