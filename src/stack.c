#include "defs.h"

void
push(struct atom *p)
{
	if (tos < 0 || tos + 1 > STACKSIZE)
		kaput("stack error");

	stack[tos++] = p;

	if (tos > max_stack)
		max_stack = tos; // new high
}

struct atom *
pop(void)
{
	if (tos < 1 || tos > STACKSIZE)
		kaput("stack error");

	return stack[--tos];
}

void
save(void)
{
	if (interrupt)
		kaput("interrupt");

	if (tof < 0 || tof + 10 > FRAMESIZE)
		kaput("frame error, circular definition?");

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
		max_frame = tof; // new high
}

void
restore(void)
{
	if (tof < 10 || tof > FRAMESIZE)
		kaput("frame error");

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
save_symbol(struct atom *p)
{
	if (tof < 0 || tof + 2 > FRAMESIZE)
		kaput("frame error, circular definition?");

	frame[tof + 0] = get_binding(p);
	frame[tof + 1] = get_usrfunc(p);

	tof += 2;

	if (tof > max_frame)
		max_frame = tof; // new high

	set_binding(p, symbol(NIL));
	set_usrfunc(p, symbol(NIL));
}

void
restore_symbol(struct atom *p)
{
	if (tof < 2 || tof > FRAMESIZE)
		kaput("frame error");

	tof -= 2;

	set_binding(p, frame[tof + 0]);
	set_usrfunc(p, frame[tof + 1]);
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
