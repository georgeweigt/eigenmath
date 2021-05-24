function
combine_tensors(h)
{
	var i, p1, T;
	T = symbol(NIL);
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(T)) {
				add_tensors(T, p1);
				T = pop();
			} else
				T = p1;
			stack.splice(i, 1);
			i--; // use same index again
		}
	}
	return T;
}
