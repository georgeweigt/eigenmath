function
power_tensor(BASE, EXPO)
{
	var i, n, p1;

	p1 = copy_tensor(BASE);

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		push(p1.elem[i]);
		push(EXPO);
		power();
		p1.elem[i] = pop();
	}

	push(p1);
}
