function
eval_prefixform(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();
	outbuf = "";
	prefixform(p1);
	push_string(outbuf);
}
