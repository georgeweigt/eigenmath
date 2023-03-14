function
eval_mod(p1)
{
	push(cadr(p1));
	evalf();
	push(caddr(p1));
	evalf();
	modfunc();
}

function
modfunc()
{
	var d1, d2, i, n, p1, p2;

	p2 = pop();
	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			push(p2);
			modfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!isnum(p1) || !isnum(p2) || iszero(p2)) {
		push_symbol(MOD);
		push(p1);
		push(p2);
		list(3);
		return;
	}

	if (isrational(p1) && isrational(p2)) {
		mod_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 % d2);
}

function
mod_rationals(p1, p2)
{
	if (isinteger(p1) && isinteger(p2)) {
		mod_integers(p1, p2);
		return;
	}
	push(p1);
	push(p1);
	push(p2);
	divide();
	absfunc();
	floorfunc();
	push(p2);
	multiply();
	if (p1.sign == p2.sign)
		negate();
	add();
}

function
mod_integers(p1, p2)
{
	var a, b;
	a = bignum_mod(p1.a, p2.a);
	b = bignum_int(1);
	push_bignum(p1.sign, a, b);
}
