function
eval_log(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	log();
	expanding = t;
}
