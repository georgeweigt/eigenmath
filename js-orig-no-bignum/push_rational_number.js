const MAXINT = 1e15;

function
push_rational_number(sign, a, b)
{
	if (a < MAXINT && b < MAXINT)
		push({sign:sign, a:a, b:b});
	else
		push_double(sign * a / b);
}
