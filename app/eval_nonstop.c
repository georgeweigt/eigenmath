#include "app.h"

// not reentrant due to jmpbuf1

void
eval_nonstop(void)
{
	// these have to be volatile, crash occurs otherwise

	int volatile save_tos;
	int volatile save_tof;
	int volatile save_expanding;

	if (setjmp(jmpbuf1)) {
		jmpsel = 0;
		tos = save_tos;
		tof = save_tof;
		expanding = save_expanding;
		restore();
		pop();
		push_symbol(NIL);
		return;
	}

	save();

	save_tos = tos;
	save_tof = tof;
	save_expanding = expanding;

	jmpsel = 1;
	eval();
	jmpsel = 0;

	restore();
}
