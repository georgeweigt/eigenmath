function
lengthf(p)
{
	var n = 0;
	while (iscons(p)) {
		n++;
		p = cdr(p);
	}
	return n;
}
