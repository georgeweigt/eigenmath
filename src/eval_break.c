void
eval_break(struct atom *p1)
{
	breakflag = 1;
	push_symbol(NIL);
}
