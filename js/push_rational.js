function
push_rational(a, b)
{
	if (b == 0)
		stop("divide by zero");

	if (a == 0)
		b = 1;

	if (b < 0) {
		a = -a;
		b = -b;
	}

	if (Math.abs(a) > 1e10 || b > 1e10)
		push_double(a / b);
	else
		push({a:a, b:b});
}
