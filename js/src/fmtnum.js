function
fmtnum(n)
{
	n = Math.abs(n);

	if (n > 0 && n < 0.0001)
		return n.toExponential(5);
	else
		return n.toPrecision(6);
}
