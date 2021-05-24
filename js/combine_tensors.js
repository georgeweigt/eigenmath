function
combine_tensors(h)
{
	var i, p1, T;
	T = symbol(NIL);
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(T)) {
				push(T);
				push(p1);
				add_tensors();
				T = pop();
			} else
				T = p1;
			stack.splice(i, 1);
			i--; // use same index again
		}
	}
	return T;
}
