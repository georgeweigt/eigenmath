#include "app.h"

void
eval_nonstop(void)
{
	// these have to be volatile, crash occurs otherwise

	int volatile save_tos;
	int volatile save_tof;
	int volatile save_expanding;

	if (setjmp(jmpbuf2)) {
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

	drawing = 2;
	eval();
	drawing = 1;

	restore();
}
