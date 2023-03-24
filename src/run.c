char *trace1;
char *trace2;

void
run(char *buf)
{
	if (setjmp(jmpbuf0))
		return;

	tos = 0;
	tof = 0;
	toj = 0;
	interrupt = 0;
	eval_level = 0;
	loop_level = 0;
	expanding = 1;
	drawing = 0;
	journaling = 0;

	if (zero == NULL) {
		init_symbol_table();
		init_bignums();
		push_symbol(POWER);
		push_integer(-1);
		push_rational(1, 2);
		list(3);
		imaginaryunit = pop();
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

		gc_check(); // see gc.c for note about garbage collection

		s = scan_input(s); // also updates trace1 and trace2

		if (s == NULL)
			break; // end of input

		dupl();
		evalf();

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
	trace1 = s;
	s = scan(s);
	if (s) {
		trace2 = s;
		trace_input();
	}
	return s;
}

void
eval_run(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	p1 = pop();

	if (!isstr(p1))
		stopf("run: file name expected");

	run_file(p1->u.str);

	push_symbol(NIL);
}

void
run_file(char *filename)
{
	char *buf;
	struct atom *p;

	p = alloc_str();

	buf = read_file(filename);

	if (buf == NULL)
		stopf("run: cannot read file");

	p->u.str = buf; // buf is freed on next gc

	push(p); // protect buf from garbage collection

	loop_level++;
	run_buf(buf);
	loop_level--;

	pop();
}

void
trace_input(void)
{
	char c, *s;
	struct atom *p1;
	p1 = get_binding(symbol(TRACE));
	if (p1 == symbol(NIL) || iszero(p1))
		return;
	c = 0;
	s = trace1;
	outbuf_init();
	while (*s && s < trace2) {
		c = *s++;
		outbuf_putc(c);
	}
	if (c != '\n')
		outbuf_puts("\n");
	printbuf(outbuf, BLUE);
}

// suppress blank lines

void
print_input_line(void)
{
	char c, *s;
	c = '\n';
	s = trace1;
	outbuf_init();
	while (*s && s < trace2) {
		if (*s == '\n' && c == '\n') {
			s++;
			continue;
		}
		c = *s++;
		outbuf_putc(c);
	}
	if (c != '\n')
		outbuf_puts("\n");
	printbuf(outbuf, RED);
}

void
print_scan_line(char *s)
{
	trace2 = s;
	print_input_line();
}

char *init_script =
"i = sqrt(-1)\n"
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
	if (journaling)
		longjmp(jmpbuf1, 1);
	print_input_line();
	snprintf(strbuf, STRBUFLEN, "Stop: %s\n", s);
	printbuf(strbuf, RED);
	longjmp(jmpbuf0, 1);
}

// kaput stops even in eval_nonstop()

void
kaput(char *s)
{
	journaling = 0;
	stopf(s);
}
