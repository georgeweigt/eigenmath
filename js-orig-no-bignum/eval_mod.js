function
eval_mod(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	mod();
	expanding = t;
}
