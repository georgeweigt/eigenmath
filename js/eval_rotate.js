function
eval_rotate(p1)
{
	var c, m, n, opcode, phase, psi, t;

	t = expanding;
	expanding = 1;

	push(cadr(p1));
	evalf();
	psi = pop();

	if (!istensor(psi) || psi.dim.length > 1 || (psi.elem.length & (psi.elem.length - 1)) != 0)
		stopf("rotate");

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (!iscons(cdr(p1)))
			stopf("rotate");
		opcode = car(p1);
		push(cadr(p1));
		evalf();
		c = pop_integer();
		p1 = cddr(p1);

		if (opcode == symbol("C")) {
			if (!iscons(cdr(p1)))
				stopf("rotate");
			opcode = car(p1);
			push(cadr(p1));
			evalf();
			n = pop_integer();
			p1 = cddr(p1);
		} else
			n = c;

		psi = rotate_check(psi, c);
		psi = rotate_check(psi, n);

		if (opcode == symbol("H")) {
			rotate_h(psi, n);
			continue;
		}

		if (opcode == symbol("P")) {
			if (!iscons(p1))
				stopf("rotate");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			push(imaginaryunit);
			multiply();
			exp();
			phase = pop();
			rotate_p(psi, c, n, phase);
			continue;
		}

		if (opcode == symbol("Q")) {
			rotate_q(psi, n);
			continue;
		}

		if (opcode == symbol("R")) {
			rotate_r(psi, n);
			continue;
		}

		if (opcode == symbol("S")) {
			m = n;
			if (!iscons(p1))
				stopf("rotate");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			n = pop_integer();
			psi = rotate_check(psi, n);
			rotate_s(psi, c, m, n);
			continue;
		}

		if (opcode == symbol("X")) {
			rotate_x(psi, c, n);
			continue;
		}

		if (opcode == symbol("Y")) {
			rotate_y(psi, c, n);
			continue;
		}

		if (opcode == symbol("Z")) {
			rotate_z(psi, c, n);
			continue;
		}

		stopf("rotate");
	}

	push(psi);

	expanding = t;
}

function
rotate_check(psi, n)
{
	var i, t;

	if (n < 0 || n > 11)
		stopf("rotate");

	n = 1 << (n + 1);

	if (n > psi.elem.length) {
		t = alloc_tensor();
		t.dim.push(n);
		for (i = 0; i < n; i++)
			t.elem.push(zero);
		for (i = 0; i < psi.elem.length; i++)
			t.elem[i] = psi.elem[i];
		psi = t;
	}

	return psi;
}

// hadamard

function
rotate_h(psi, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if (i & n) {
			push(psi.elem[i ^ n]);		// KET0
			push(psi.elem[i]);		// KET1
			add();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			push(psi.elem[i ^ n]);		// KET0
			push(psi.elem[i]);		// KET1
			subtract();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			psi.elem[i] = pop();		// KET1
			psi.elem[i ^ n] = pop();	// KET0
		}
}

// phase

function
rotate_p(psi, c, n, phase)
{
	var i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if ((i & c) && (i & n)) {
			push(psi.elem[i]);		// KET1
			push(phase);
			multiply();
			psi.elem[i] = pop();		// KET1
		}
}

// swap

function
rotate_s(psi, c, m, n)
{
	var i;
	c = 1 << c;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if ((i & c) && (i & m) && !(i & n)) {
			push(psi.elem[i]);
			push(psi.elem[i ^ m ^ n]);
			psi.elem[i] = pop();
			psi.elem[i ^ m ^ n] = pop();
		}
}

// pauli x

function
rotate_x(psi, c, n)
{
	var i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if ((i & c) && (i & n)) {
			push(psi.elem[i ^ n]);		// KET0
			push(psi.elem[i]);		// KET1
			psi.elem[i ^ n] = pop();	// KET0
			psi.elem[i] = pop();		// KET1
		}
}

// pauli y

function
rotate_y(psi, c, n)
{
	var i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if ((i & c) && (i & n)) {
			push(imaginaryunit);
			negate();
			push(psi.elem[i ^ n]);		// KET0
			multiply();
			push(imaginaryunit);
			push(psi.elem[i]);		// KET1
			multiply();
			psi.elem[i ^ n] = pop();	// KET0
			psi.elem[i] = pop();		// KET1
		}
}

// pauli z

function
rotate_z(psi, c, n)
{
	var i;
	c = 1 << c;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++)
		if ((i & c) && (i & n)) {
			push(psi.elem[i]);		// KET1
			negate();
			psi.elem[i] = pop();		// KET1
		}
}

// quantum fourier transform

function
rotate_q(psi, n)
{
	var i, j, phase;
	for (i = n; i >= 0; i--) {
		rotate_h(psi, i);
		for (j = 0; j < i; j++) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			evalf();
			multiply_factors(3);
			exp();
			phase = pop();
			rotate_p(psi, j, i, phase);
		}
	}
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_s(psi, i, i, n - i);
}

// inverse qft

function
rotate_r(psi, n)
{
	var i, j, phase;
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_s(psi, i, i, n - i);
	for (i = 0; i <= n; i++) {
		for (j = i - 1; j >= 0; j--) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			evalf();
			multiply_factors(3);
			negate();
			exp();
			phase = pop();
			rotate_p(psi, j, i, phase);
		}
		rotate_h(psi, i);
	}
}
