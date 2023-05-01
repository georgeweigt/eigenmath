function
cmp_tensors(p1, p2)
{
	var i, t;

	t = p1.dim.length - p2.dim.length;

	if (t)
		return t;

	for (i = 0; i < p1.dim.length; i++) {
		t = p1.dim[i] - p2.dim[i];
		if (t)
			return t;
	}

	for (i = 0; i < p1.elem.length; i++) {
		t = cmp(p1.elem[i], p2.elem[i]);
		if (t)
			return t;
	}

	return 0;
}
