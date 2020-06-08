#include "defs.h"

void
push(struct atom *p)
{
	if (tos < 0 || tos > STACKSIZE)
		stop("stack error 1");
	if (tos == STACKSIZE)
		stop("stack full");
	stack[tos++] = p;
	if (tos > max_stack)
		max_stack = tos;
}

struct atom *
pop(void)
{
	if (tos < 1 || tos > STACKSIZE)
		stop("stack error 2");
	return stack[--tos];
}

void
save(void)
{
	if (interrupt)
		stop("Interrupt");
	if (tof < 0 || tof > FRAMESIZE)
		stop("frame error 1");
	if (tof + 10 > FRAMESIZE)
		stop("frame error, circular definition?");
	frame[tof + 0] = p0;
	frame[tof + 1] = p1;
	frame[tof + 2] = p2;
	frame[tof + 3] = p3;
	frame[tof + 4] = p4;
	frame[tof + 5] = p5;
	frame[tof + 6] = p6;
	frame[tof + 7] = p7;
	frame[tof + 8] = p8;
	frame[tof + 9] = p9;
	tof += 10;
	if (tof > max_frame)
		max_frame = tof;
}

void
restore(void)
{
	if (tof < 10 || tof > FRAMESIZE)
		stop("frame error 2");
	tof -= 10;
	p0 = frame[tof + 0];
	p1 = frame[tof + 1];
	p2 = frame[tof + 2];
	p3 = frame[tof + 3];
	p4 = frame[tof + 4];
	p5 = frame[tof + 5];
	p6 = frame[tof + 6];
	p7 = frame[tof + 7];
	p8 = frame[tof + 8];
	p9 = frame[tof + 9];
}

void
save_binding(struct atom *p)
{
	if (tof < 0 || tof > FRAMESIZE)
		stop("frame error 3");
	if (tof + 2 > FRAMESIZE)
		stop("frame error, circular definition?");
	frame[tof + 0] = binding[p - symtab];
	frame[tof + 1] = arglist[p - symtab];
	tof += 2;
	if (tof > max_frame)
		max_frame = tof;
}

void
restore_binding(struct atom *p)
{
	if (tof < 2 || tof > FRAMESIZE)
		stop("frame error 4");
	tof -= 2;
	binding[p - symtab] = frame[tof + 0];
	arglist[p - symtab] = frame[tof + 1];
}

void
swap(void)
{
	struct atom *p1, *p2; // ok, no gc before push
	p1 = pop();
	p2 = pop();
	push(p1);
	push(p2);
}

void
push_string(char *s)
{
	struct atom *p; // ok, no gc before push
	p = alloc();
	p->k = STR;
	p->u.str = strdup(s);
	if (p->u.str == NULL)
		malloc_kaput();
	push(p);
	string_count++;
}
