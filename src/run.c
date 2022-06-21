#include "defs.h"

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

		if (alloc_count > BLOCKSIZE * block_count / 5) {
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

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		kaput("malloc");
	}

	p1 = alloc(); // do this so gc can free the buf in case of stop
	p1->k = STR;
	p1->u.str = buf;
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
	outbuf_index = 0;
	while (*s && s < trace2) {
		c = *s++;
		print_char(c);
	}
	if (c != '\n')
		print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLUE);
}

// suppress blank lines

void
print_input_line(void)
{
	char c, *s;
	c = '\n';
	s = trace1;
	outbuf_index = 0;
	while (*s && s < trace2) {
		if (*s == '\n' && c == '\n') {
			s++;
			continue;
		}
		c = *s++;
		print_char(c);
	}
	if (c != '\n')
		print_char('\n');
	print_char('\0');
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

	outbuf_index = 0;

	sprintf(tbuf, "block_count %d (%d%%)\n", block_count, 100 * block_count / MAXBLOCKS);
	print_str(tbuf);

	sprintf(tbuf, "free_count %d\n", free_count);
	print_str(tbuf);

	sprintf(tbuf, "gc_count %d\n", gc_count);
	print_str(tbuf);

	sprintf(tbuf, "bignum_count %d\n", bignum_count);
	print_str(tbuf);

	sprintf(tbuf, "ksym_count %d\n", ksym_count);
	print_str(tbuf);

	sprintf(tbuf, "usym_count %d\n", usym_count);
	print_str(tbuf);

	sprintf(tbuf, "string_count %d\n", string_count);
	print_str(tbuf);

	sprintf(tbuf, "tensor_count %d\n", tensor_count);
	print_str(tbuf);

	sprintf(tbuf, "max_level %d\n", max_level);
	print_str(tbuf);

	sprintf(tbuf, "max_stack %d (%d%%)\n", max_stack, 100 * max_stack / STACKSIZE);
	print_str(tbuf);

	sprintf(tbuf, "max_frame %d (%d%%)\n", max_frame, 100 * max_frame / FRAMESIZE);
	print_str(tbuf);

	sprintf(tbuf, "max_journal %d (%d%%)\n", max_journal, 100 * max_journal / JOURNALSIZE);
	print_str(tbuf);

	print_char('\0');

	printbuf(outbuf, BLACK);

	push_symbol(NIL);
}

char *init_script[] = {
	"i=sqrt(-1)",
	"last=0",
	"tty=0",
	"cross(u,v)=dot(u,(((0,0,0),(0,0,-1),(0,1,0)),((0,0,1),(0,0,0),(-1,0,0)),((0,-1,0),(1,0,0),(0,0,0))),v)",
	"curl(u)=(d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))",
	"div(u)=d(u[1],x)+d(u[2],y)+d(u[3],z)",
	"ln(x)=log(x)",
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
	sprintf(tbuf, "Stop: %s\n", s);
	printbuf(tbuf, RED);
	longjmp(jmpbuf0, 1);
}

// kaput stops even in eval_nonstop()

void
kaput(char *s)
{
	journaling = 0;
	stop(s);
}
