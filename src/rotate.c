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
	int control, target;

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

		OPCODE = car(p1);
		push(cadr(p1));
		eval();
		target = pop_integer();
		p1 = cddr(p1);

		control = target;

		if (OPCODE == symbol(C_LOWER)) {
			OPCODE = car(p1);
			push(cadr(p1));
			eval();
			target = pop_integer();
			p1 = cddr(p1);
		}

		rotate_check(control, target);

		if (OPCODE == symbol(H_LOWER)) {
			rotate_h(control, target);
			continue;
		}

		if (OPCODE == symbol(P_LOWER)) {
			push(imaginaryunit);
			push(car(p1));
			eval();
			multiply();
			exponential();
			PHASE = pop();
			p1 = cdr(p1);
			rotate_p(control, target);
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
rotate_check(int control, int target)
{
	int i, n;

	if (control < 0 || control > 11 || target < 0 || target > 11)
		stop("rotate");

	if (control > target)
		n = 1 << (control + 1);
	else
		n = 1 << (target + 1);

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
