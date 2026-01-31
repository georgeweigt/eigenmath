function
eval_expform(p1)
{
	push(cadr(p1));
	evalf();
	expform(1);
}

function
expform(flag)
{
	var h, i, n, p1, num, den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			expform(flag);
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			expform(flag);
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {

		push(p1);
		numden();
		num = pop();
		den = pop();

		p1 = num;
		if (car(p1) == symbol(MULTIPLY)) {
			h = stack.length;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform(flag);
				p1 = cdr(p1);
			}
			multiply_factors(stack.length - h);
		} else {
			push(p1);
			expform(flag);
		}
		num = pop();

		p1 = den;
		if (car(p1) == symbol(MULTIPLY)) {
			h = stack.length;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform(flag);
				p1 = cdr(p1);
			}
			multiply_factors(stack.length - h);
		} else {
			push(p1);
			expform(flag);
		}
		den = pop();

		push(num);
		push(den);
		divide();
		return;
	}

	if (car(p1) == symbol(POWER)) {
		push(cadr(p1));
		expform(flag);
		push(caddr(p1));
		expform(flag);
		power();
		return;
	}

	if (car(p1) == symbol(COS)) {
		push(cadr(p1));
		expform(flag);
		expcos();
		return;
	}

	if (car(p1) == symbol(SIN)) {
		push(cadr(p1));
		expform(flag);
		expsin();
		return;
	}

	if (car(p1) == symbol(TAN)) {
		push(cadr(p1));
		expform(flag);
		exptan();
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		expform(flag);
		expcosh();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		expform(flag);
		expsinh();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		expform(flag);
		exptanh();
		return;
	}

	if (flag) {

		if (car(p1) == symbol(ARCCOS)) {
			scan("-i log(z + i sqrt(1 - abs(z)^2))", 0);
			push_symbol(Z_LOWER);
			push(cadr(p1));
			expform(1);
			subst();
			evalf();
			return;
		}

		if (car(p1) == symbol(ARCSIN)) {
			scan("-i log(i z + sqrt(1 - abs(z)^2))", 0);
			push_symbol(Z_LOWER);
			push(cadr(p1));
			expform(1);
			subst();
			evalf();
			return;
		}

		if (car(p1) == symbol(ARCTAN)) {
			push(cadr(p1)); // y
			expform(1);
			num = pop();
			push(caddr(p1)); // x
			expform(1);
			den = pop();
			if (iszero(num) || iszero(den)) {
				push_symbol(ARCTAN);
				push(num);
				push(den);
				list(3);
			} else {
				scan("-1/2 i log((i - z) / (i + z))", 0);
				push_symbol(Z_LOWER);
				push(num);
				push(den);
				divide();
				subst();
				evalf();
			}
			return;
		}

		if (car(p1) == symbol(ARCCOSH)) {
			scan("log(z + sqrt(abs(z)^2 - 1))", 0);
			push_symbol(Z_LOWER);
			push(cadr(p1));
			expform(1);
			subst();
			evalf();
			return;
		}

		if (car(p1) == symbol(ARCSINH)) {
			scan("log(z + sqrt(abs(z)^2 + 1))", 0);
			push_symbol(Z_LOWER);
			push(cadr(p1));
			expform(1);
			subst();
			evalf();
			return;
		}

		if (car(p1) == symbol(ARCTANH)) {
			scan("1/2 log((1 + z) / (1 - z))", 0);
			push_symbol(Z_LOWER);
			push(cadr(p1));
			expform(1);
			subst();
			evalf();
			return;
		}
	}

	h = stack.length;
	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		expform(flag);
		p1 = cdr(p1);
	}
	list(stack.length - h);
	evalf();
}
