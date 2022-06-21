#include "defs.h"

void
eval_print(struct atom *p1)
{
	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(car(p1));
		eval();
		print_result();
		p1 = cdr(p1);
	}

	push_symbol(NIL);
}

void
print_result(void)
{
	struct atom *p1, *p2;

	p2 = pop(); // result
	p1 = pop(); // input

	if (p2 == symbol(NIL))
		return;

	if (issymbol(p1))
		prep_symbol_equals(p1, p2);

	if (iszero(get_binding(symbol(TTY)))) {
		push(p2);
		display();
		return;
	}

	print_infixform(p2);
}

void
prep_symbol_equals(struct atom *p1, struct atom *p2)
{
	if (p1 == p2)
		return; // A = A

	if (iskeyword(p1))
		return; // keyword like "float"

	if (p1 == symbol(I_LOWER) && isimaginaryunit(p2))
		return;

	if (p1 == symbol(J_LOWER) && isimaginaryunit(p2))
		return;

	push_symbol(SETQ);
	push(p1);
	push(p2);
	list(3);
	p2 = pop();
}

void
print_str(char *s)
{
	while (*s)
		print_char(*s++);
}

void
print_char(int c)
{
	if (outbuf_index == outbuf_length) {
		outbuf_length += 1000;
		outbuf = (char *) realloc(outbuf, outbuf_length);
		if (outbuf == NULL)
			kaput("malloc");
	}
	outbuf[outbuf_index++] = c;
}
