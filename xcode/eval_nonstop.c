#include "app.h"

void
eval_nonstop(void)
{
	if (nonstop) {
		pop();
		push_symbol(NIL);
		return; // not reentrant
	}

	nonstop = 1;
	eval_nonstop_nib();
	nonstop = 0;
}

void
eval_nonstop_nib(void)
{
	// these have to be volatile, crash occurs otherwise

	int volatile save_tos, save_eval_level, save_gc_level, save_expanding;

	if (setjmp(jmpbuf1)) {

		tos = save_tos;

		eval_level = save_eval_level;
		gc_level = save_gc_level;
		expanding = save_expanding;

		push_symbol(NIL); // return value

		return;
	}

	save_tos = tos - 1;

	save_eval_level = eval_level;
	save_gc_level = gc_level;
	save_expanding = expanding;

	evalf();
}
