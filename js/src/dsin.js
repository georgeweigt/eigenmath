function
dsin(F, X)
{
	push(cadr(F));
	push(X);
	derivative();
	push(cadr(F));
	cosfunc();
	multiply();
}
