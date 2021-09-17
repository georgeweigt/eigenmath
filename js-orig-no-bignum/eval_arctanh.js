function
eval_arctanh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arctanh();
	expanding = t;
}
