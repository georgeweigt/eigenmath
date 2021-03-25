function
eval_infixform(p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();
	outbuf = "";
	infixform_expr(p1);
	push_string(outbuf);
}
