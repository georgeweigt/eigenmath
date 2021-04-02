function
eval_mag(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	mag();
	expanding = t;
}
