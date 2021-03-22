#include "defs.h"

#undef PSI
#undef OPCODE
#undef PHASE
#undef T
#undef N
#undef KET0
#undef KET1

#define PSI p3
#define OPCODE p4
#define PHASE p5
#define T p6

#define N PSI->u.tensor->nelem
#define KET0 PSI->u.tensor->elem[i ^ target]
#define KET1 PSI->u.tensor->elem[i]

void
eval_rotate(void)
{
	int control, target, target2;

	push(cadr(p1));
	eval();
	PSI = pop();

	if (!istensor(PSI)) {
		PSI = alloc_tensor(2);
		PSI->u.tensor->ndim = 1;
		PSI->u.tensor->dim[0] = 2;
		push_integer(1);
		PSI->u.tensor->elem[0] = pop();
	}

	p1 = cddr(p1);

	while (iscons(p1)) {

		if (length(p1) < 2)
			stop("rotate");

		OPCODE = car(p1);
		push(cadr(p1));
		eval();
		target = pop_integer();
		p1 = cddr(p1);

		control = target;

		if (OPCODE == symbol(C_LOWER)) {
			if (length(p1) < 2)
				stop("rotate");
			OPCODE = car(p1);
			push(cadr(p1));
			eval();
			target = pop_integer();
			p1 = cddr(p1);
		}

		rotate_check(control);
		rotate_check(target);

		if (OPCODE == symbol(H_LOWER)) {
			rotate_h(control, target);
			continue;
		}

		if (OPCODE == symbol(P_LOWER)) {
			if (length(p1) < 1)
				stop("rotate");
			push(car(p1));
			p1 = cdr(p1);
			eval();
			push(imaginaryunit);
			multiply();
			exponential();
			PHASE = pop();
			rotate_p(control, target);
			continue;
		}

		if (OPCODE == symbol(S_LOWER)) {
			if (length(p1) < 1)
				stop("rotate");
			push(car(p1));
			p1 = cdr(p1);
			eval();
			target2 = pop_integer();
			rotate_check(target2);
			rotate_s(target, target2);
			continue;
		}

		if (OPCODE == symbol(X_LOWER)) {
			rotate_x(control, target);
			continue;
		}

		if (OPCODE == symbol(Y_LOWER)) {
			rotate_y(control, target);
			continue;
		}

		if (OPCODE == symbol(Z_LOWER)) {
			rotate_z(control, target);
			continue;
		}

		stop("rotate");
	}

	push(PSI);
}

void
rotate_check(int n)
{
	int i;

	if (n < 0 || n > 11)
		stop("rotate");

	n = 1 << (n + 1);

	if (n > N) {
		T = alloc_tensor(n);
		T->u.tensor->ndim = 1;
		T->u.tensor->dim[0] = n;
		for (i = 0; i < N; i++)
			T->u.tensor->elem[i] = PSI->u.tensor->elem[i];
		PSI = T;
	}
}

void
rotate_h(int control, int target)
{
	int i;
	control = 1 << control;
	target = 1 << target;
	for (i = 0; i < N; i++)
		if ((i & control) && (i & target)) {
			push(KET0);
			push(KET1);
			add();
			push_rational(1, 2);
			sqrtv();
			multiply();
			push(KET0);
			push(KET1);
			subtract();
			push_rational(1, 2);
			sqrtv();
			multiply();
			KET1 = pop();
			KET0 = pop();
		}
}

void
rotate_p(int control, int target)
{
	int i;
	control = 1 << control;
	target = 1 << target;
	for (i = 0; i < N; i++)
		if ((i & control) && (i & target)) {
			push(KET1);
			push(PHASE);
			multiply();
			KET1 = pop();
		}
}

void
rotate_s(int m, int n)
{
	int i;
	m = 1 << m;
	n = 1 << n;
	for (i = 0; i < N; i++)
		if ((i & m) && !(i & n)) {
			push(PSI->u.tensor->elem[i]);
			push(PSI->u.tensor->elem[i ^ m ^ n]);
			PSI->u.tensor->elem[i] = pop();
			PSI->u.tensor->elem[i ^ m ^ n] = pop();
		}
}

void
rotate_x(int control, int target)
{
	int i;
	control = 1 << control;
	target = 1 << target;
	for (i = 0; i < N; i++)
		if ((i & control) && (i & target)) {
			push(KET0);
			push(KET1);
			KET0 = pop();
			KET1 = pop();
		}
}

void
rotate_y(int control, int target)
{
	int i;
	control = 1 << control;
	target = 1 << target;
	for (i = 0; i < N; i++)
		if ((i & control) && (i & target)) {
			push(imaginaryunit);
			negate();
			push(KET0);
			multiply();
			push(imaginaryunit);
			push(KET1);
			multiply();
			KET0 = pop();
			KET1 = pop();
		}
}

void
rotate_z(int control, int target)
{
	int i;
	control = 1 << control;
	target = 1 << target;
	for (i = 0; i < N; i++)
		if ((i & control) && (i & target)) {
			push(KET1);
			negate();
			KET1 = pop();
		}
}
