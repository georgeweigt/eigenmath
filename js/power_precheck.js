function
power_precheck(BASE, EXPO)
{
	// 1^expr = expr^0 = 1

	if (isplusone(BASE) || iszero(EXPO)) {
		if (isdouble(BASE) || isdouble(EXPO))
			push_double(1.0);
		else
			push_integer(1);
		return 1;
	}

	// expr^1 = expr

	if (isplusone(EXPO)) {
		push(BASE);
		return 1;
	}

	// 0^expr

	if (iszero(BASE)) {
		if (isnum(EXPO)) {
			if (isdouble(BASE) || isdouble(EXPO))
				push_double(0.0);
			else
				push_integer(0);
		} else {
			push_symbol(POWER);
			push(BASE);
			push(EXPO);
			list(3);
		}
		return 1;
	}

	return 0;
}
