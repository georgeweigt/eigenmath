function
power_minusone(EXPO)
{
	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push_integer(-1);
		push(EXPO);
		list(3);
		return;
	}

	// do this for better performance

	if (equalq(EXPO, 1, 2)) {
		push(imaginaryunit);
		return;
	}

	if (isrational(EXPO))
		power_minusone_rational(EXPO);
	else
		power_minusone_double(EXPO);
}

function
power_minusone_rational(EXPO)
{
	var n, R;

	// R = EXPO mod 2

	push(EXPO);
	push_integer(2);
	mod();
	R = pop();

	// convert negative rotation to positive

	if (R.a < 0) {
		push(R);
		push_integer(2);
		add();
		R = pop();
	}

	push(R);
	push_integer(2);
	multiply();
	floor();
	n = pop_integer(); // number of 90 degree turns

	push(R);
	push_integer(n);
	push_rational(1, 2);
	multiply();
	subtract();
	R = pop(); // remainder

	switch (n) {

	case 0:
		if (iszero(R))
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
		}
		break;

	case 1:
		if (iszero(R))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(1, 2);
			subtract();
			list(3);
			list(3);
		}
		break;

	case 2:
		if (iszero(R))
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (iszero(R)) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push(R);
			push_rational(1, 2);
			subtract();
			list(3);
		}
		break;
	}
}

function
power_minusone_double(EXPO)
{
	var expo, n, r;

	expo = EXPO.d;

	// expo = expo mod 2

	expo = expo % 2;

	// convert negative rotation to positive

	if (expo < 0)
		expo += 2;

	n = Math.floor(2 * expo); // number of 90 degree turns

	r = expo - n / 2; // remainder

	switch (n) {

	case 0:
		if (r == 0)
			push_integer(1);
		else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
		}
		break;

	case 1:
		if (r == 0)
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
			list(3);
		}
		break;

	case 2:
		if (r == 0)
			push_integer(-1);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push_symbol(POWER);
			push_integer(-1);
			push_double(r);
			list(3);
			list(3);
		}
		break;

	case 3:
		if (r == 0) {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		} else {
			push_symbol(POWER);
			push_integer(-1);
			push_double(r - 0.5);
			list(3);
		}
		break;
	}
}
