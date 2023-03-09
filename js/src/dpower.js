//	     v
//	y = u
//
//	log y = v log u
//
//	1 dy   v du           dv
//	- -- = - -- + (log u) --
//	y dx   u dx           dx
//
//	dy    v  v du           dv
//	-- = u  (- -- + (log u) --)
//	dx       u dx           dx

function
dpower(F, X)
{
	if (isnum(cadr(F)) && isnum(caddr(F))) {
		push_integer(0); // irr or imag
		return;
	}

	push(caddr(F));		// v/u
	push(cadr(F));
	divide();

	push(cadr(F));		// du/dx
	push(X);
	derivative();

	multiply();

	push(cadr(F));		// log u
	logfunc();

	push(caddr(F));		// dv/dx
	push(X);
	derivative();

	multiply();

	add();
	push(F);		// u^v
	multiply();
}
