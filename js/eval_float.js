function
eval_float(p1)
{
	push(cadr(p1));
	evalf();
	floatf();
}
