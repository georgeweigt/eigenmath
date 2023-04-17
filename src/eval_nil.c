void
eval_nil(struct atom *p1)
{
	(void) p1; // silence compiler
	push_symbol(NIL);
}
