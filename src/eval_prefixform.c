void
eval_prefixform(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	outbuf_init();
	prefixform(p1);

	push_string(outbuf);
}

// for debugging

void
print_prefixform(struct atom *p)
{
	outbuf_init();
	prefixform(p);
	outbuf_puts("\n");
	printbuf(outbuf, BLACK);
}

void
prefixform(struct atom *p)
{
	char *s;
	switch (p->atomtype) {
	case CONS:
		outbuf_puts("(");
		prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf_puts(" ");
			prefixform(car(p));
			p = cdr(p);
		}
		if (p != symbol(NIL)) {
			outbuf_puts(" . ");
			prefixform(p);
		}
		outbuf_puts(")");
		break;
	case STR:
		outbuf_puts("\"");
		outbuf_puts(p->u.str);
		outbuf_puts("\"");
		break;
	case RATIONAL:
		if (p->sign == MMINUS)
			outbuf_puts("-");
		s = mstr(p->u.q.a);
		outbuf_puts(s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		outbuf_puts("/");
		outbuf_puts(s);
		break;
	case DOUBLE:
		snprintf(strbuf, STRBUFLEN, "%g", p->u.d);
		outbuf_puts(strbuf);
		if (!strchr(strbuf, '.') && !strchr(strbuf, 'e'))
			outbuf_puts(".0");
		break;
	case KSYM:
	case USYM:
		outbuf_puts(printname(p));
		break;
	case TENSOR:
		outbuf_puts("(tensor)");
		break;
	default:
		outbuf_puts("(?)");
		break;
	}
}
