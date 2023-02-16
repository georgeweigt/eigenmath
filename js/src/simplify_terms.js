function
simplify_terms(h)
{
	var i, n = 0, p1, p2;
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (isradicalterm(p1)) {
			push(p1);
			evalf();
			p2 = pop();
			if (!equal(p1, p2)) {
				stack[i] = p2;
				n++;
			}
		}
	}
	return n;
}
