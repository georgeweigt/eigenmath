function
eval_testge(p1)
{
	if (cmp_args(p1) >= 0)
		push_integer(1);
	else
		push_integer(0);
}
