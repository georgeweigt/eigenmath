function
eval_rotate(p1)
{
	var c, m, n, opcode, phase, psi;

	push(cadr(p1));
	evalf();
	psi = pop();

	if (!istensor(psi) || psi.dim.length > 1 || psi.elem.length > 32768 || (psi.elem.length & (psi.elem.length - 1)) != 0)
		stopf("rotate error 1 first argument is not a vector or dimension error");

	c = 0;

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (!iscons(cdr(p1)))
			stopf("rotate error 2 unexpected end of argument list");

		opcode = car(p1);
		push(cadr(p1));
		evalf();
		n = pop_integer();

		if (n > 14 || (1 << n) >= psi.elem.length)
			stopf("rotate error 3 qubit number format or range");

		p1 = cddr(p1);

		if (opcode == symbol("C")) {
			c |= 1 << n;
			continue;
		}

		if (opcode == symbol("H")) {
			rotate_h(psi, c, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("P")) {
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			push(imaginaryunit);
			multiply();
			exp();
			phase = pop();
			rotate_p(psi, c, n, phase);
			c = 0;
			continue;
		}

		if (opcode == symbol("Q")) {
			rotate_q(psi, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("V")) {
			rotate_v(psi, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("W")) {
			m = n;
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			n = pop_integer();
			if (n > 14 || (1 << n) >= psi.elem.length)
				stopf("rotate error 3 qubit number format or range");
			rotate_w(psi, c, m, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("X")) {
			rotate_x(psi, c, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("Y")) {
			rotate_y(psi, c, n);
			c = 0;
			continue;
		}

		if (opcode == symbol("Z")) {
			rotate_z(psi, c, n);
			c = 0;
			continue;
		}

		stopf("rotate error 4 unknown rotation code");
	}

	push(psi);
}

// hadamard

function
rotate_h(psi, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
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
}

// phase

function
rotate_p(psi, c, n, phase)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(psi.elem[i]);		// KET1
			push(phase);
			multiply();
			psi.elem[i] = pop();		// KET1
		}
	}
}

// swap

function
rotate_w(psi, c, m, n)
{
	var i;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if ((i & m) && !(i & n)) {
			push(psi.elem[i]);
			push(psi.elem[i ^ m ^ n]);
			psi.elem[i] = pop();
			psi.elem[i ^ m ^ n] = pop();
		}
	}
}

// pauli x

function
rotate_x(psi, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(psi.elem[i ^ n]);		// KET0
			push(psi.elem[i]);		// KET1
			psi.elem[i ^ n] = pop();	// KET0
			psi.elem[i] = pop();		// KET1
		}
	}
}

// pauli y

function
rotate_y(psi, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
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
}

// pauli z

function
rotate_z(psi, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < psi.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(psi.elem[i]);		// KET1
			negate();
			psi.elem[i] = pop();		// KET1
		}
	}
}

// quantum fourier transform

function
rotate_q(psi, n)
{
	var i, j, phase;
	for (i = n; i >= 0; i--) {
		rotate_h(psi, 0, i);
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
			rotate_p(psi, 1 << j, i, phase);
		}
	}
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(psi, 0, i, n - i);
}

// inverse qft

function
rotate_v(psi, n)
{
	var i, j, phase;
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(psi, 0, i, n - i);
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
			rotate_p(psi, 1 << j, i, phase);
		}
		rotate_h(psi, 0, i);
	}
}
