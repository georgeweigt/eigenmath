#include "app.h"

void
eval_nonstop(void)
{
	if (shuntflag) {
		pop();
		push_symbol(NIL);
		return; // not reentrant
	}
	shuntflag = 1;
	errorflag = 0;
	evalf();
	if (errorflag) {
		errorflag = 0;
		pop();
		push_symbol(NIL);
	}
	shuntflag = 0;
}
