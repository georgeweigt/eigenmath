void
eval_do(struct atom *p1)
{
	push_symbol(NIL);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop(); // discard previous result
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
}
