function
gcd_integers(a, b)
{
	var t;
	a = Math.abs(a);
	b = Math.abs(b);
	while (b) {
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}
