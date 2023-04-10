void
eval_binding(struct atom *p1)
{
	push(get_binding(cadr(p1)));
}
