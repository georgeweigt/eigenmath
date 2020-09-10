// subst(new, old, target)

function
eval_subst(p1)
{
	var p2, p3, p4;

	p1 = cdr(p1);

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	p4 = pop();

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	p3 = pop();

	push(car(p1));
	p1 = cdr(p1);
	evalf();
	p2 = pop();

	push(p2);
	push(p3);
	push(p4);
	subst();
	evalf(); // normalize
}
