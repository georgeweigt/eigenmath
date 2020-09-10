//  1   number
//  2   number to power (root)
//  3   -1 to power (imaginary)
//  4   other factor (symbol, power, func, etc)
//  5   exponential
//  6   derivative

function
order_factor(p)
{
	if (isnum(p))
		return 1;

	if (p == symbol(EXP1))
		return 5;

	if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
		return 6;

	if (car(p) == symbol(POWER)) {

		p = cadr(p); // p = base

		if (isminusone(p))
			return 3; // base = -1

		if (isnum(p))
			return 2;

		if (p == symbol(EXP1))
			return 5;

		if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
			return 6;
	}

	return 4;
}
