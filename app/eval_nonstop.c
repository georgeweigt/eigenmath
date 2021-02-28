#include "app.h"

void
eval_nonstop(void)
{
	if (journaling) {
		pop();
		push_symbol(NIL);
		return; // not reentrant
	}

	journaling = 1;

	save();
	eval_nonstop_nib();
	restore();

	journaling = 0;
	toj = 0;
}

void
eval_nonstop_nib(void)
{
	// these have to be volatile, crash occurs otherwise

	int volatile save_tos;
	int volatile save_tof;
	int volatile t;

	if (setjmp(jmpbuf1)) {
		undo(); // restore symbol table
		tos = save_tos;
		tof = save_tof;
		expanding = t;
		push_symbol(NIL);
		return;
	}

	save_tos = tos - 1;
	save_tof = tof;
	t = expanding;

	eval();
}
