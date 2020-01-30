#include "defs.h"

char *trace_ptr;
char *trace_ptr0;

void
run(char *s)
{
	trace_ptr = s;
	trace_ptr0 = s;

	if (setjmp(stop_return))
		return;

	init_globals();

	while (1) {

		if (iszero(binding[AUTOEXPAND]))
			expanding = 0;
		else
			expanding = 1;

		s = scan(s, 0);

		if (s == NULL)
			break; // end of input

		trace_input(s);

		eval_and_print_result(1);

		check_stack();

		if (clear_flag)
			clear();
	}
}

void
check_stack(void)
{
	if (tos != 0)
		stop("stack error");
	if (tof != 0)
		stop("frame error");
}

void
echo_input(char *s)
{
	term_flag = BLUE;
	printstr(s);
	printstr("\n");
	term_flag = BLACK;
}

void
stop(char *s)
{
	if (draw_flag == 2)
		longjmp(draw_stop_return, 1);
	else {
		trace_error();
		term_flag = RED;
		if (s == NULL)
			printstr("Stop\n");
		else {
			printstr("Stop: ");
			printstr(s);
			printstr("\n");
		}
		term_flag = BLACK;
		gc();
		longjmp(stop_return, 1);
	}
}

char *init_script[] = {
	"e=exp(1)",
	"i=sqrt(-1)",
	"autoexpand=1",
	"trange=(-pi,pi)",
	"xrange=(-10,10)",
	"yrange=(-10,10)",
	"last=0",
	"trace=0",
	"tty=0",
	"cross(u,v)=(u[2]*v[3]-u[3]*v[2],u[3]*v[1]-u[1]*v[3],u[1]*v[2]-u[2]*v[1])",
	"curl(u)=(d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))",
	"div(u)=d(u[1],x)+d(u[2],y)+d(u[3],z)",
	"ln(x)=log(x)",
};

void
clear(void)
{
	int i, n;

	init_symbol_table();

	init_globals();

	push_integer(0);
	zero = pop();

	push_integer(1);
	one = pop();

	push_integer(-1);
	minusone = pop();

	push_symbol(POWER);
	push_integer(-1);
	push_rational(1, 2);
	list(3);
	imaginaryunit = pop();

	n = sizeof init_script / sizeof (char *);

	for (i = 0; i < n; i++) {
		scan(init_script[i], 0);
		eval();
		pop();
	}

	gc();
}

void
init_globals(void)
{
	stop_flag = 0;
	draw_flag = 0;
	term_flag = BLACK;
	clear_flag = 0;

	tos = 0;
	tof = 0;

	p0 = symbol(NIL);
	p1 = symbol(NIL);
	p2 = symbol(NIL);
	p3 = symbol(NIL);
	p4 = symbol(NIL);
	p5 = symbol(NIL);
	p6 = symbol(NIL);
	p7 = symbol(NIL);
	p8 = symbol(NIL);
	p9 = symbol(NIL);

	set_binding(symbol(TRACE), zero); // start with trace disabled
}

void
print_status(void)
{
	sprintf(tbuf, "block_count %d\n", block_count);
	printstr(tbuf);
	sprintf(tbuf, "free_count %d\n", free_count);
	printstr(tbuf);
	sprintf(tbuf, "gc_count %d\n", gc_count);
	printstr(tbuf);
	sprintf(tbuf, "bignum_count %d\n", bignum_count);
	printstr(tbuf);
	sprintf(tbuf, "string_count %d\n", string_count);
	printstr(tbuf);
	sprintf(tbuf, "tensor_count %d\n", tensor_count);
	printstr(tbuf);
	sprintf(tbuf, "max_stack %d (%d%%)\n", max_stack, 100 * max_stack / STACKSIZE);
	printstr(tbuf);
	sprintf(tbuf, "max_frame %d (%d%%)\n", max_frame, 100 * max_frame / FRAMESIZE);
	printstr(tbuf);
}

void
trace_input(char *s)
{
	char c, *t;

	trace_ptr0 = trace_ptr;

	while (*trace_ptr && trace_ptr < s) {
		// advance to next line
		do
			trace_ptr++;
		while (*trace_ptr && trace_ptr[-1] != '\n');
	}

	if (iszero(binding[TRACE]))
		return;

	c = '\n';

	t = trace_ptr0;

	while (t < trace_ptr) {
		c = *t++;
		printchar(c);
	}

	if (c != '\n')
		printchar('\n');
}

void
trace_error(void)
{
	char *s;
	if (iszero(binding[TRACE])) {
		binding[TRACE] = one;
		s = trace_ptr;
		trace_ptr = trace_ptr0;
		trace_input(s);
	}
}

void
eval_run(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	if (!isstr(p1))
		stop("string expected");

	run_file(p1->u.str);

	push_symbol(NIL);
}

void
run_file(char *filename)
{
	int fd, n;
	char *buf, *ptr, *ptr0, *s;

	fd = open(filename, O_RDONLY, 0);

	if (fd == -1)
		stop("cannot open file");

	// get file size

	n = (int) lseek(fd, 0, SEEK_END);

	if (n < 0) {
		close(fd);
		stop("lseek error");
	}

	lseek(fd, 0, SEEK_SET);

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		malloc_kaput();
	}

	push_string(buf);
	p1 = pop();

	if (read(fd, buf, n) != n) {
		close(fd);
		stop("read error");
	}

	close(fd);

	buf[n] = 0;

	s = buf;

	ptr = trace_ptr;
	ptr0 = trace_ptr0;

	trace_ptr = s;
	trace_ptr0 = s;

	while (1) {

		s = scan(s, 0);

		if (s == NULL)
			break; // end of input

		trace_input(s);

		eval_and_print_result(1);

		if (clear_flag)
			stop("clear not allowed in run file");
	}

	trace_ptr = ptr;
	trace_ptr0 = ptr0;

	p1->u.str = strdup("NULL");

	if (p1->u.str == NULL)
		malloc_kaput();

	free(buf);
}
