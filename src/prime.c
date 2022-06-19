//	Look up the nth prime
//
//	Input:		n on stack (0 < n < 10001)
//
//	Output:		nth prime on stack

#include "defs.h"

void
eval_prime(struct atom *p1)
{
	push(cadr(p1));
	eval();
	prime();
}

void
prime(void)
{
	int n;
	n = pop_integer();
	if (n < 1 || n > MAXPRIMETAB)
		stop("prime: arg out of range");
	n = primetab[n - 1];
	push_integer(n);
}
