function
reduce_radical_factors(h, coeff)
{
	if (iszero(coeff))
		return coeff;

	if (!any_radical_factors(h))
		return;

	if (isrational(coeff))
		return reduce_radical_rational(h, coeff);
	else
		return reduce_radical_double(h, coeff);
}
