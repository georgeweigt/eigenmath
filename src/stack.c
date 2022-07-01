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
save_symbol(struct atom *p)
{
	if (tof < 0 || tof + 2 > FRAMESIZE)
		kaput("frame error");

	frame[tof + 0] = get_binding(p);
	frame[tof + 1] = get_usrfunc(p);

	tof += 2;

	if (tof > max_frame)
		max_frame = tof; // new high
}

void
restore_symbol(struct atom *p)
{
	if (tof < 2 || tof > FRAMESIZE)
		kaput("frame error");

	tof -= 2;

	set_symbol(p, frame[tof + 0], frame[tof + 1]);
}

void
swap(void)
{
	struct atom *p1, *p2;
	p1 = pop();
	p2 = pop();
	push(p1);
	push(p2);
}

void
push_string(char *s)
{
	struct atom *p;
	p = alloc_atom();
	s = strdup(s);
	if (s == NULL)
		exit(1);
	p->atomtype = STR;
	p->u.str = s;
	push(p);
	string_count++;
}
