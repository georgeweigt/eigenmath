function
push_rational_number(sign, a, b)
{
	if (a < MAXINT && b < MAXINT)
		push({sign:sign, a:sign * a, b:b});
	else
		push_double(sign * a / b);
}
