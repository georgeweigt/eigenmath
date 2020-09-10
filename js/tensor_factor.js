function
tensor_factor(h)
{
	var i, n, p1, p2;
	p2 = symbol(NIL);
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(p2))
				stop("tensor * tensor not supported, use dot or outer instead");
			p2 = p1;
			stack.splice(i, 1); // remove factor
			i--;
			n--;
		}
	}
	return p2;
}
