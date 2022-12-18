function
annotate_result(p1, p2)
{
	if (!isusersymbol(p1))
		return 0;

	if (p1 == p2)
		return 0; // A = A

	if (p1 == symbol(I_LOWER) && isimaginaryunit(p2))
		return 0;

	if (p1 == symbol(J_LOWER) && isimaginaryunit(p2))
		return 0;

	return 1;
}
