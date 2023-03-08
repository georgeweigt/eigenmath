function
eval_erf(p1)
{
	push(cadr(p1));
	evalf();
	erffunc();
}

function
erffunc()
{
	var d, p1;

	p1 = pop();

	if (isdouble(p1)) {
		d = erf(p1.d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
		return;
	}

	push_symbol(ERF);
	push(p1);
	list(2);
}

// https://hewgill.com/picomath/javascript/erf.js.html

function erf(x) {

	// added this
	if (x == 0)
		return 0;

	// constants
	var a1 = 0.254829592;
	var a2 = -0.284496736;
	var a3 = 1.421413741;
	var a4 = -1.453152027;
	var a5 = 1.061405429;
	var p = 0.3275911;

	// Save the sign of x
	var sign = 1;
	if (x < 0) {
		sign = -1;
	}
	x = Math.abs(x);

	// A&S formula 7.1.26
	var t = 1.0/(1.0 + p*x);
	var y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*Math.exp(-x*x);

	return sign*y;
}
