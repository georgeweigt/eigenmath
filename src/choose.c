#include "defs.h"

void
eval_choose(void)
{
	push(cadr(p1)); // n
	eval();
	push(caddr(p1)); // k
	eval();
	choose();
}

// Result vanishes for k < 0 or k > n. (A=B, p. 19)

void
choose(void)
{
	save();
	choose_nib();
	restore();
}

#undef N
#undef K

#define N p1
#define K p2

void
choose_nib(void)
{
	K = pop();
	N = pop();

	if (choose_check_args() == 0) {
		push(zero);
		return;
	}

	push(N);
	factorial();

	push(K);
	factorial();

	divide();

	push(N);
	push(K);
	subtract();
	factorial();

	divide();
}

int
choose_check_args(void)
{
	if (isnum(N) && lessp(N, zero))
		return 0;
	else if (isnum(K) && lessp(K, zero))
		return 0;
	else if (isnum(N) && isnum(K) && lessp(N, K))
		return 0;
	else
		return 1;
}
