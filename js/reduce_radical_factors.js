function
reduce_radical_factors(h, COEFF)
{
	if (!any_radical_factors(h))
		return COEFF;

	if (isrational(COEFF))
		return reduce_radical_rational(h, COEFF);
	else
		return reduce_radical_double(h, COEFF);
}
