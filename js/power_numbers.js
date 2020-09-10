// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var base, expo;

	if (isminusone(BASE)) {
		power_imaginary_unit(BASE, EXPO);
		return;
	}

	if (isnegativenumber(BASE)) {
		power_imaginary_unit(BASE, EXPO);
		push(BASE);
		negate();
		BASE = pop();
		power_numbers();
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
