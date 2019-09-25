// Push expression factors onto the stack. For example...
//
// Input
//
//       2
//     3x  + 2x + 1
//
// Output on stack
//
//     [  3  ]
//     [ x^2 ]
//     [  2  ]
//     [  x  ]
//     [  1  ]
//
// but not necessarily in that order. Returns the number of factors.

#include "defs.h"

// Local "struct atom *" is OK here because no functional path to garbage collector.

int
factors(struct atom *p)
{
	int h = tos;
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			push_term_factors(car(p));
			p = cdr(p);
		}
	} else
		push_term_factors(p);
	return tos - h;
}

// Local "struct atom *" is OK here because no functional path to garbage collector.

void
push_term_factors(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			push(car(p));
			p = cdr(p);
		}
	} else
		push(p);
}
