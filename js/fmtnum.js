function
fmtnum(n)
{
	if (Math.abs(n) > 0 && Math.abs(n) < 0.0001)
		return Math.abs(n).toExponential(5);
	else
		return Math.abs(n).toPrecision(6);
}
