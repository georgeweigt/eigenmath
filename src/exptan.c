// tan(z) = (i - i exp(2 i z)) / (exp(2 i z) + 1)

void
eval_exptan(struct atom *p1)
{
	push(cadr(p1));
	eval();
	exptan();
}

void
exptan(void)
{
	struct atom *p1;

	push_integer(2);
	push(imaginaryunit);
	multiply_factors(3);
	expfunc();

	p1 = pop();

	push(imaginaryunit);
	push(imaginaryunit);
	push(p1);
	multiply();
	subtract();

	push(p1);
	push_integer(1);
	add();

	divide();
}
