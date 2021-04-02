function
eval_tanh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	tanh();
	expanding = t;
}
