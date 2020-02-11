#include "defs.h"

void
eval_mathjax(void)
{
	push(cadr(p1));
	eval();

	mathjax();

	push_string(outbuf);
}

void
mathjax(void)
{
	save();
	mathjax_nib();
	restore();
}

void
mathjax_nib(void)
{
	outbuf_index = 0;

	p1 = pop();

	if (isstr(p1))
		mml_string(p1, 0);
	else {
		print_str("$$\n");
		latex_expr(p1);
		print_str("\n$$");
	}

	print_char('\0');
}
