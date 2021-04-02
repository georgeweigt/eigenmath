function
eval_exptanh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	exptanh();
	expanding = t;
}
