function
eval_exp(p1)
{
	push(cadr(p1));
	evalf();
	expfunc();
}

function
expfunc()
{
	push_symbol(EXP1);
	swap();
	power();
}
