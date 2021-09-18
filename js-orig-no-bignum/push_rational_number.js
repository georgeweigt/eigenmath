function
push_rational_number(sign, a, b)
{
	if (sign < 0 && a > 0) //FIXME
		a = -a;

	if (sign > 0 && a < 0) //FIXME
		a = -a;

	if (Math.abs(a) < MAXINT && b < MAXINT)
		push({sign:sign, a:a, b:b});
	else
		push_double(a / b); //FIXME
}
