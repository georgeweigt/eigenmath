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

	p2 = prep_result(p1, p2);

	if (iszero(get_binding(symbol(TTY)))) {
		push(p2);
		display();
	} else
		print_infixform(p2);
}

// if a user symbol A was evaluated, print A = result

struct atom *
prep_result(struct atom *p1, struct atom *p2)
{
	if (!isusersymbol(p1))
		return p2;

	if (p1 == p2)
		return p2; // A = A

	if (p1 == symbol(I_LOWER) && isimaginaryunit(p2))
		return p2;

	if (p1 == symbol(J_LOWER) && isimaginaryunit(p2))
		return p2;

	push_symbol(SETQ);
	push(p1);
	push(p2);
	list(3);
	p2 = pop();

	return p2;
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
			exit(1);
	}
	outbuf[outbuf_index++] = c;
}
