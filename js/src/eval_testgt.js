function
eval_testgt(p1)
{
	if (relop(p1) > 0)
		push_integer(1);
	else
		push_integer(0);
}
