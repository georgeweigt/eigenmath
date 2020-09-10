function
eval_prefixform(p1)
{
	var s;
	push(cadr(p1));
	evalf();
	p1 = pop();
	s = prefixform(p1);
	push_string(s);
}
