function
dcos(F, X)
{
	push(cadr(F));
	push(X);
	derivative();
	push(cadr(F));
	sin();
	multiply();
	negate();
}
