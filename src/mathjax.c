#include "defs.h"

void
eval_mathjax(struct atom *p1)
{
	push(cadr(p1));
	eval();

	mathjax();

	push_string(outbuf);
}

void
mathjax(void)
{
	struct atom *p1;
	p1 = pop();

	outbuf_index = 0;

	if (isstr(p1))
		mml_string(p1, 0);
	else {
		print_str("$$\n");
		latex_expr(p1);
		print_str("\n$$");
	}

	print_char('\0');
}
