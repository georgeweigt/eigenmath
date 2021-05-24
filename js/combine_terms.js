// terms are combined by adding numerical coefficients

function
combine_terms(h)
{
	var i;
	sort_terms(h);
	for (i = h; i < stack.length - 1; i++) {
		if (combine_terms_nib(i, i + 1)) {
			if (iszero(stack[i]))
				stack.splice(i, 2); // remove 2
			else
				stack.splice(i + 1, 1); // remove 1
			i--; // use same index again
		}
	}
}
