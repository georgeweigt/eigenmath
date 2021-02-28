#include "app.h"

void
eval_nonstop(void)
{
	// these have to be volatile, crash occurs otherwise

	int volatile save_tos;
	int volatile save_tof;
	int volatile save_expanding;

	if (journaling) {
		pop();
		push_symbol(NIL);
		return; // not reentrant
	}

	if (setjmp(jmpbuf1)) {
		journaling = 0;
		undo(); // restore symbol table
		tos = save_tos;
		tof = save_tof;
		expanding = save_expanding;
		restore();
		push_symbol(NIL);
		return;
	}

	save();

	save_tos = tos - 1;
	save_tof = tof;
	save_expanding = expanding;

	journaling = 1;
	eval();
	journaling = 0;
	toj = 0;

	restore();
}
