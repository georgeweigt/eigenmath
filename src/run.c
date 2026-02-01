void
run(char *buf)
{
	if (setjmp(jmpbuf))
		return;

	tos = 0;
	interrupt = 0;
	eval_level = 0;
	gc_level = 0;
	expanding = 1;
	drawing = 0;
	shuntflag = 0;
	errorflag = 0;
	breakflag = 0;

	if (zero == NULL) {
		srand((unsigned) time(NULL));
		init_symbol_table();
		push_bignum(MPLUS, mint(0), mint(1));
		zero = pop();
		push_bignum(MPLUS, mint(1), mint(1));
		one = pop();
		push_bignum(MMINUS, mint(1), mint(1));
		minusone = pop();
		push_symbol(POWER);
		push_integer(-1);
		push_rational(1, 2);
		list(3);
		imaginaryunit = pop();
		binding[symbol(I_LOWER)->u.usym.index] = imaginaryunit;
		usrfunc[symbol(I_LOWER)->u.usym.index] = symbol(NIL);
		run_init_script();
	}

	run_buf(buf);
}

void
run_buf(char *buf)
{
	char *s, *save_trace1, *save_trace2;
	struct atom *p1;

	save_trace1 = trace1;
	save_trace2 = trace2;

	s = buf;

	for (;;) {

		s = scan_input(s); // also updates trace1 and trace2

		if (s == NULL)
			break; // end of input

		dupl();
		evalg();

		// update last

		dupl();
		p1 = pop();

		if (p1 != symbol(NIL))
			set_symbol(symbol(LAST), p1, symbol(NIL));

		print_result();
	}

	trace1 = save_trace1;
	trace2 = save_trace2;
}

char *
scan_input(char *s)
{
	struct atom *p1;
	trace1 = s;
	s = scan(s);
	trace2 = s;
	p1 = get_binding(symbol(TRACE));
	if (p1 != symbol(TRACE) && !iszero(p1))
		print_trace(BLUE);
	return s;
}

void
print_trace(int color)
{
	char c, *s;
	if (trace1 == NULL || trace2 == NULL)
		return;
	outbuf_init();
	c = '\n';
	s = trace1;
	while (*s && s < trace2) {
		c = *s++;
		outbuf_putc(c);
	}
	if (c != '\n')
		outbuf_putc('\n');
	printbuf(outbuf, color);
}

char *init_script =
"grad(f) = d(f,(x,y,z))\n"
"cross(a,b) = (dot(a[2],b[3])-dot(a[3],b[2]),dot(a[3],b[1])-dot(a[1],b[3]),dot(a[1],b[2])-dot(a[2],b[1]))\n"
"curl(u) = (d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))\n"
"div(u) = d(u[1],x)+d(u[2],y)+d(u[3],z)\n"
"laguerre(x,n,m) = (n + m)! sum(k,0,n,(-x)^k / ((n - k)! (m + k)! k!))\n"
"legendre(f,n,m,x) = eval(1 / (2^n n!) (1 - x^2)^(m/2) d((x^2 - 1)^n,x,n + m),x,f)\n"
"hermite(x,n) = (-1)^n exp(x^2) d(exp(-x^2),x,n)\n"
"binomial(n,k) = n! / k! / (n - k)!\n"
"choose(n,k) = n! / k! / (n - k)!\n"
;

void
run_init_script(void)
{
	run_buf(init_script);
}

void
stopf(char *s)
{
	print_trace(RED);
	snprintf(strbuf, STRBUFLEN, "Stop: %s\n", s);
	printbuf(strbuf, RED);
	longjmp(jmpbuf, 1);
}
