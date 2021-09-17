function
eval_conj(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	conj();
	expanding = t;
}
