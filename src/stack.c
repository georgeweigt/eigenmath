void
push(struct atom *p)
{
	if (tos < 0 || tos >= STACKSIZE)
		stopf("stack error, circular definition?");
	stack[tos++] = p;
	if (tos > max_tos)
		max_tos = tos;
}

struct atom *
pop(void)
{
	if (tos < 1 || tos > STACKSIZE)
		stopf("stack error");
	return stack[--tos];
}

void
fpush(struct atom *p)
{
	if (tof < 0 || tof >= FRAMESIZE)
		stopf("frame error, circular definition?");
	frame[tof++] = p;
	if (tof > max_tof)
		max_tof = tof;
}

struct atom *
fpop(void)
{
	if (tof < 1 || tof > FRAMESIZE)
		stopf("frame error");
	return frame[--tof];
}

void
save_symbol(struct atom *p)
{
	fpush(p);
	fpush(get_binding(p));
	fpush(get_usrfunc(p));
}

void
restore_symbol(void)
{
	struct atom *p1, *p2, *p3;
	p3 = fpop();
	p2 = fpop();
	p1 = fpop();
	set_symbol(p1, p2, p3);
}

void
dupl(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	push(p1);
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
	p = alloc_str();
	s = strdup(s);
	if (s == NULL)
		exit(1);
	p->u.str = s;
	push(p);
}
