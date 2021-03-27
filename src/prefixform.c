#include "defs.h"

void
eval_prefixform(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	outbuf_index = 0;
	prefixform(p1);
	print_char('\0');

	push_string(outbuf);
}

// for debugging

void
print_prefixform(struct atom *p)
{
	outbuf_index = 0;
	prefixform(p);
	print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLACK);
}

void
prefixform(struct atom *p)
{
	char buf[24], *s;
	switch (p->k) {
	case CONS:
		print_char('(');
		prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_str(" ");
			prefixform(car(p));
			p = cdr(p);
		}
		if (p != symbol(NIL)) {
			print_str(" . ");
			prefixform(p);
		}
		print_char(')');
		break;
	case STR:
		print_char('"');
		print_str(p->u.str);
		print_char('"');
		break;
	case RATIONAL:
		if (p->sign == MMINUS)
			print_char('-');
		s = mstr(p->u.q.a);
		print_str(s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		print_char('/');
		print_str(s);
		break;
	case DOUBLE:
		sprintf(buf, "%g", p->u.d);
		print_str(buf);
//		for debugging
//		if (!strchr(buf, '.') && !strchr(buf, 'e') && !strchr(buf, 'E'))
//			print_str(".0");
		break;
	case KSYM:
	case USYM:
		print_str(printname(p));
		break;
	case TENSOR:
		print_str("(tensor)");
		break;
	default:
		print_str("(?)");
		break;
	}
}
