function
emit_number(p)
{
	if (isrational(p))
		emit_rational(p);
	else
		emit_double(p);
}
