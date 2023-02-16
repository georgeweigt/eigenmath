function
combine_factors(h)
{
	var i, n;
	sort_factors_provisional(h);
	n = stack.length;
	for (i = h; i < n - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			stack.splice(i + 1, 1); // remove factor
			i--; // use same index again
			n--;
		}
	}
}
