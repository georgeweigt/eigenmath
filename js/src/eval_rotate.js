function
eval_rotate(p1)
{
	var m, n, c, PSI, OPCODE, PHASE;

	push(cadr(p1));
	evalf();
	PSI = pop();

	if (!istensor(PSI) || PSI.dim.length > 1 || PSI.elem.length > 32768 || (PSI.elem.length & (PSI.elem.length - 1)) != 0)
		stopf("rotate error 1 first argument is not a vector or dimension error");

	c = 0;

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (!iscons(cdr(p1)))
			stopf("rotate error 2 unexpected end of argument list");

		OPCODE = car(p1);
		push(cadr(p1));
		evalf();
		n = pop_integer();

		if (n > 14 || (1 << n) >= PSI.elem.length)
			stopf("rotate error 3 qubit number format or range");

		p1 = cddr(p1);

		if (OPCODE == symbol("C")) {
			c |= 1 << n;
			continue;
		}

		if (OPCODE == symbol("H")) {
			rotate_h(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("P")) {
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			push(imaginaryunit);
			multiply();
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("Q")) {
			rotate_q(PSI, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("V")) {
			rotate_v(PSI, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("W")) {
			m = n;
			if (!iscons(p1))
				stopf("rotate error 2 unexpected end of argument list");
			push(car(p1));
			p1 = cdr(p1);
			evalf();
			n = pop_integer();
			if (n > 14 || (1 << n) >= PSI.elem.length)
				stopf("rotate error 3 qubit number format or range");
			rotate_w(PSI, c, m, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("X")) {
			rotate_x(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("Y")) {
			rotate_y(PSI, c, n);
			c = 0;
			continue;
		}

		if (OPCODE == symbol("Z")) {
			rotate_z(PSI, c, n);
			c = 0;
			continue;
		}

		stopf("rotate error 4 unknown rotation code");
	}

	push(PSI);
}

// hadamard

function
rotate_h(PSI, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(PSI.elem[i ^ n]);		// KET0
			push(PSI.elem[i]);		// KET1
			add();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			push(PSI.elem[i ^ n]);		// KET0
			push(PSI.elem[i]);		// KET1
			subtract();
			push_rational(1, 2);
			sqrtfunc();
			multiply();
			PSI.elem[i] = pop();		// KET1
			PSI.elem[i ^ n] = pop();	// KET0
		}
	}
}

// phase

function
rotate_p(PSI, PHASE, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(PSI.elem[i]);		// KET1
			push(PHASE);
			multiply();
			PSI.elem[i] = pop();		// KET1
		}
	}
}

// swap

function
rotate_w(PSI, c, m, n)
{
	var i;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if ((i & m) && !(i & n)) {
			push(PSI.elem[i]);
			push(PSI.elem[i ^ m ^ n]);
			PSI.elem[i] = pop();
			PSI.elem[i ^ m ^ n] = pop();
		}
	}
}

function
rotate_x(PSI, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(PSI.elem[i ^ n]);		// KET0
			push(PSI.elem[i]);		// KET1
			PSI.elem[i ^ n] = pop();	// KET0
			PSI.elem[i] = pop();		// KET1
		}
	}
}

function
rotate_y(PSI, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(imaginaryunit);
			push(PSI.elem[i ^ n]);		// KET0
			multiply();
			push(imaginaryunit);
			negate();
			push(PSI.elem[i]);		// KET1
			multiply();
			PSI.elem[i ^ n] = pop();	// KET0
			PSI.elem[i] = pop();		// KET1
		}
	}
}

function
rotate_z(PSI, c, n)
{
	var i;
	n = 1 << n;
	for (i = 0; i < PSI.elem.length; i++) {
		if ((i & c) != c)
			continue;
		if (i & n) {
			push(PSI.elem[i]);		// KET1
			negate();
			PSI.elem[i] = pop();		// KET1
		}
	}
}

// quantum fourier transform

function
rotate_q(PSI, n)
{
	var i, j, PHASE;
	for (i = n; i >= 0; i--) {
		rotate_h(PSI, 0, i);
		for (j = 0; j < i; j++) {
			push_rational(1, 2);
			push_integer(i - j);
			power();
			push(imaginaryunit);
			push_symbol(PI);
			evalf();
			multiply_factors(3);
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, 1 << j, i);
		}
	}
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(PSI, 0, i, n - i);
}

// inverse qft

function
rotate_v(PSI, n)
{
	var i, j, PHASE;
	for (i = 0; i < (n + 1) / 2; i++)
		rotate_w(PSI, 0, i, n - i);
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
			expfunc();
			PHASE = pop();
			rotate_p(PSI, PHASE, 1 << j, i);
		}
		rotate_h(PSI, 0, i);
	}
}
