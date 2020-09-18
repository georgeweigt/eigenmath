// congruent terms are combined by adding numerical coefficients

function
combine_terms(h)
{
	var i, n;
	sort_terms(h);
	n = stack.length;
	for (i = h; i < n - 1; i++) {
		if (combine_terms_nib(i, i + 1)) {
			if (!istensor(stack[i]) && iszero(stack[i]))
				stack.splice(i, 2); // remove 2
			else
				stack.splice(i + 1, 1); // remove 1
			i--; // use same index again
			n = stack.length;
		}
	}
	if (stack.length - h == 1 && !istensor(stack[h]) && iszero(stack[h]))
		stack.splice(h); // all terms canceled
}
