void
eval_quote(struct atom *p1)
{
	push(cadr(p1)); // not evaluated
}
