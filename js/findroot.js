// coefficients are on the stack

function
findroot(h)
{
	var i, j, k, m, n;
	var C, T, X;

	C = stack[h]; // constant term

	if (!isrational(C))
		stop("root finder");

	if (iszero(C)) {
		push_integer(0);
		return 1;
	}

	k = stack.length;
	push(C);
	numerator();
	n = pop_integer();
	divisors(n); // push divisors of n

	m = stack.length;
	push(C);
	denominator();
	n = pop_integer();
	divisors(n); // push divisors of n

	for (i = k; i < m; i++) {
		for (j = m; j < stack.length; j++) {

			push(stack[i]);
			push(stack[j]);
			divide();
			X = pop();

			findroot_eval(h, k - h, X);

			T = pop();

			if (iszero(T)) {
				stack.splice(k); // pop all
				push(X);
				return 1;
			}

			push(X);
			negate();
			X = pop();

			findroot_eval(h, k - h, X);

			T = pop();

			if (iszero(T)) {
				stack.splice(k); // pop all
				push(X);
				return 1;
			}
		}
	}

	stack.splice(k); // pop all

	return 0; // no root
}

// evaluate p(x) at x = X using horner's rule

function
findroot_eval(h, n, X)
{
	var i;

	push(stack[h + n - 1]);

	for (i = n - 1; i > 0; i--) {
		push(X);
		multiply();
		push(stack[h + i - 1]);
		add();
	}
}
