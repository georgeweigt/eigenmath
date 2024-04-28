function
combine_factors(h)
{
	var i;
	sort_factors_provisional(h);
	for (i = h; i < stack.length - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			stack.splice(i + 1, 1); // remove factor
			i--; // use same index again
		}
	}
}
