// does f depend on x?

function
dependent(f, x)
{
	if (equal(f, x))
		return 1;

	// a user function with no arguments?

	if (isusersymbol(car(f)) && lengthf(f) == 1)
		return 1;

	while (iscons(f)) {
		if (dependent(car(f), x))
			return 1;
		f = cdr(f);
	}

	return 0;
}
