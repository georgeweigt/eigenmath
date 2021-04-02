function
eval_det(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	det();
	expanding = t;
}
