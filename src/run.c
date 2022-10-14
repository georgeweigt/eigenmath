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

		if (free_count < BLOCKSIZE * block_count / 2)
			gc();

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
		stop("run: file name expected");

	run_file(p1->u.str);

	push_symbol(NIL);
}

void
run_file(char *filename)
{
	int fd, n;
	char *buf, *s, *t1, *t2;
	struct atom *p1;

	fd = open(filename, O_RDONLY, 0);

	if (fd == -1)
		stop("run: cannot open file");

	// get file size

	n = (int) lseek(fd, 0, SEEK_END);

	if (n < 0) {
		close(fd);
		stop("run: lseek error");
	}

	lseek(fd, 0, SEEK_SET);

	p1 = alloc_atom();
	buf = malloc(n + 1);
	if (buf == NULL)
		exit(1);
	p1->atomtype = STR;
	p1->u.str = buf; // buf is freed on next gc
	string_count++;

	if (read(fd, buf, n) != n) {
		close(fd);
		stop("run: read error");
	}

	close(fd);

	buf[n] = 0;

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

void
eval_status(struct atom *p1)
{
	(void) p1; // silence compiler

	outbuf_init();

	snprintf(strbuf, STRBUFLEN, "block_count %d (%d%%)\n", block_count, 100 * block_count / MAXBLOCKS);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "free_count %d\n", free_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "gc_count %d\n", gc_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "bignum_count %d\n", bignum_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "ksym_count %d\n", ksym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "usym_count %d\n", usym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "string_count %d\n", string_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "tensor_count %d\n", tensor_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_level %d\n", max_level);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_stack %d (%d%%)\n", max_stack, 100 * max_stack / STACKSIZE);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_frame %d (%d%%)\n", max_frame, 100 * max_frame / FRAMESIZE);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_journal %d (%d%%)\n", max_journal, 100 * max_journal / JOURNALSIZE);
	outbuf_puts(strbuf);

	printbuf(outbuf, BLACK);

	push_symbol(NIL);
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
stop(char *s)
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
	stop(s);
}
