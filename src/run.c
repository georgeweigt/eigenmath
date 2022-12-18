char *trace1;
char *trace2;

void
run(char *s)
{
	if (setjmp(jmpbuf0))
		return;

	if (zero == NULL)
		init();

	prep();

	set_symbol(symbol(TRACE), zero, symbol(NIL));

	for (;;) {

		if (alloc_count > MAXBLOCKS * BLOCKSIZE / 10) {
			gc();
			alloc_count = 0;
		}

		s = scan_input(s);

		if (s == NULL)
			break; // end of input

		eval_and_print_result();

		if (tos || tof || toj)
			kaput("internal error");
	}
}

void
init(void)
{
	init_symbol_table();

	prep();

	init_bignums();

	push_symbol(POWER);
	push_integer(-1);
	push_rational(1, 2);
	list(3);
	imaginaryunit = pop();

	run_init_script();
}

void
prep(void)
{
	interrupt = 0;

	tos = 0;
	tof = 0;
	toj = 0;

	level = 0;
	expanding = 1;
	drawing = 0;
	journaling = 0;
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
eval_and_print_result(void)
{
	struct atom *p1, *p2;

	p1 = pop();
	push(p1);
	eval();
	p2 = pop();

	push(p1);
	push(p2);
	print_result();

	if (p2 != symbol(NIL))
		set_symbol(symbol(LAST), p2, symbol(NIL));
}

void
eval_run(struct atom *p1)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	if (!isstr(p1))
		stopf("run: file name expected");

	run_file(p1->u.str);

	push_symbol(NIL);
}

void
run_file(char *filename)
{
	char *buf, *s, *t1, *t2;
	struct atom *p;

	p = alloc_str();

	buf = read_file(filename);

	if (buf == NULL)
		stopf("run: cannot read file");

	p->u.str = buf; // if stop occurs, buf is freed on next gc

	s = buf;

	t1 = trace1;
	t2 = trace2;

	for (;;) {

		s = scan_input(s);

		if (s == NULL)
			break; // end of input

		eval_and_print_result();
	}

	trace1 = t1;
	trace2 = t2;

	free(buf);
	p->u.str = NULL;
}

void
trace_input(void)
{
	char c, *s;
	if (iszero(get_binding(symbol(TRACE))))
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

char *init_script[] = {
"i = sqrt(-1)",
"last = 0",
"trace = 0",
"tty = 0",
"cross(u,v) = dot(u,(((0,0,0),(0,0,-1),(0,1,0)),((0,0,1),(0,0,0),(-1,0,0)),((0,-1,0),(1,0,0),(0,0,0))),v)",
"curl(u) = (d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))",
"div(u) = d(u[1],x)+d(u[2],y)+d(u[3],z)",
"laguerre(x,n,m) = (n + m)! sum(k,0,n,(-x)^k / ((n - k)! (m + k)! k!))",
"legendre(f,n,m,x) = eval(1 / (2^n n!) (1 - x^2)^(m/2) d((x^2 - 1)^n,x,n + m),x,f)",
"hermite(x,n) = (-1)^n exp(x^2) d(exp(-x^2),x,n)",
"binomial(n,k) = n! / k! / (n - k)!",
"choose(n,k) = n! / k! / (n - k)!",
};

void
run_init_script(void)
{
	int i, n;
	char *s;
	n = sizeof init_script / sizeof (char *);
	for (i = 0; i < n; i++) {
		s = init_script[i];
		scan(s);
		eval();
		pop();
	}
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
