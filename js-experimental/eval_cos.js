function
eval_cos(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	cos();
	expanding = t;
}
