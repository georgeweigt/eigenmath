function
derf(F, X)
{
	push(cadr(F));
	push_integer(2);
	power();
	push_integer(-1);
	multiply();
	expfunc();
	push_symbol(PI);
	push_rational(-1, 2);
	power();
	multiply();
	push_integer(2);
	multiply();
	push(cadr(F));
	push(X);
	derivative();
	multiply();
}
