// tan(z) = (i - i exp(2 i z)) / (exp(2 i z) + 1)

function
eval_exptan(p1)
{
	push(cadr(p1));
	evalf();
	exptan();
}

function
exptan()
{
	var p1;

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
