function
vector(h)
{
	var n, p;

	n = stack.length - h;

	p = alloc_tensor();

	p.dim[0] = n;
	p.elem = stack.splice(h, n);

	push(p);
}
