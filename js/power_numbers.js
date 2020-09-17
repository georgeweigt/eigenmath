// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var base, expo;

	if (iszero(BASE) && isnegativenumber(EXPO))
		stopf("divide by zero");

	if (isinteger(EXPO)) {
		power_simple(BASE, EXPO);
		return;
	}

	if (isminusone(BASE)) {
		power_minusone(BASE, EXPO);
		return;
	}

	if (isnegativenumber(BASE)) {
		power_minusone(BASE, EXPO);
		push(BASE);
		negate();
		BASE = pop();
		power_numbers(BASE, EXPO);
		multiply();
		return;
	}

	if (isrational(BASE) && isrational(EXPO)) {
		power_rationals(BASE, EXPO);
		return;
	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(Math.pow(base, expo));
}
