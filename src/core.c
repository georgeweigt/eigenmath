#include "defs.h"

struct atom *
alloc(void)
{
	struct atom *p;
	if (block_count == 0)
		alloc_block();
	if (free_count == 0) {
		gc();
		if (free_count < BLOCKSIZE)
			alloc_block();
		if (free_count == 0)
			stop("out of memory");
	}
	p = free_list;
	free_list = free_list->u.cons.cdr;
	free_count--;
	return p;
}

struct atom *
alloc_vector(int nrow)
{
	struct atom *p = alloc_tensor(nrow);
	p->u.tensor->ndim = 1;
	p->u.tensor->dim[0] = nrow;
	return p;
}

struct atom *
alloc_matrix(int nrow, int ncol)
{
	struct atom *p = alloc_tensor(nrow * ncol);
	p->u.tensor->ndim = 2;
	p->u.tensor->dim[0] = nrow;
	p->u.tensor->dim[1] = ncol;
	return p;
}

struct atom *
alloc_tensor(int nelem)
{
	int i;
	struct atom *p; // ok, no gc before return
	p = alloc();
	p->k = TENSOR;
	p->u.tensor = (struct tensor *) malloc(sizeof (struct tensor) + nelem * sizeof (struct atom *));
	if (p->u.tensor == NULL)
		malloc_kaput();
	p->u.tensor->nelem = nelem;
	for (i = 0; i < nelem; i++)
		p->u.tensor->elem[i] = zero;
	tensor_count++;
	return p;
}

// garbage collector

void
gc(void)
{
	int i, j;
	struct atom *p;

	gc_count++;

	// tag everything

	for (i = 0; i < block_count; i++) {
		p = mem[i];
		for (j = 0; j < BLOCKSIZE; j++)
			p[j].tag = 1;
	}

	// untag what's used

	untag(p0);
	untag(p1);
	untag(p2);
	untag(p3);
	untag(p4);
	untag(p5);
	untag(p6);
	untag(p7);
	untag(p8);
	untag(p9);

	untag(zero);
	untag(one);
	untag(minusone);
	untag(imaginaryunit);

	// symbol table

	for (i = 0; i < 27 * NSYM; i++)
		if (symtab[i]) {
			untag(symtab[i]);
			untag(binding[i]);
			untag(arglist[i]);
		}

	for (i = 0; i < tos; i++)
		untag(stack[i]);

	for (i = 0; i < tof; i++)
		untag(frame[i]);

	// collect everything that's still tagged

	free_count = 0;

	for (i = 0; i < block_count; i++) {
		p = mem[i];
		for (j = 0; j < BLOCKSIZE; j++) {
			if (p[j].tag == 0)
				continue;
			// still tagged so it's unused, put on free list
			switch (p[j].k) {
			case KSYM:
				free(p[j].u.ksym.name);
				ksym_count--;
				break;
			case USYM:
				free(p[j].u.usym.name);
				usym_count--;
				break;
			case RATIONAL:
				mfree(p[j].u.q.a);
				mfree(p[j].u.q.b);
				break;
			case STR:
				free(p[j].u.str);
				string_count--;
				break;
			case TENSOR:
				free(p[j].u.tensor);
				tensor_count--;
				break;
			}
			p[j].k = CONS; // so no double free occurs above
			p[j].u.cons.cdr = free_list;
			free_list = p + j;
			free_count++;
		}
	}
}

void
untag(struct atom *p)
{
	int i;

	if (p == NULL)
		return; // in case gc is called before everything is initialized

	while (iscons(p)) {
		if (p->tag == 0)
			return;
		p->tag = 0;
		untag(p->u.cons.car);
		p = p->u.cons.cdr;
	}

	if (p->tag == 0)
		return;

	p->tag = 0;

	if (istensor(p))
		for (i = 0; i < p->u.tensor->nelem; i++)
			untag(p->u.tensor->elem[i]);
}

void
alloc_block(void)
{
	int i;
	struct atom *p;
	if (block_count == MAXBLOCKS)
		return;
	p = (struct atom *) malloc(BLOCKSIZE * sizeof (struct atom));
	if (p == NULL)
		return;
	mem[block_count++] = p;
	for (i = 0; i < BLOCKSIZE; i++) {
		p[i].k = CONS; // so no free in gc
		p[i].u.cons.cdr = p + i + 1;
	}
	p[BLOCKSIZE - 1].u.cons.cdr = free_list;
	free_list = p;
	free_count += BLOCKSIZE;
}

// Append one list to another.

void
append(void)
{
	int h;

	save();

	p2 = pop();
	p1 = pop();

	h = tos;

	if (iscons(p1))
		while (iscons(p1)) {
			push(car(p1));
			p1 = cdr(p1);
		}
	else
		push(p1);

	if (iscons(p2))
		while (iscons(p2)) {
			push(car(p2));
			p2 = cdr(p2);
		}
	else
		push(p2);

	list(tos - h);

	restore();
}

// Cons two things on the stack.

void
cons(void)
{
	struct atom *p; // ok, no gc before push
	p = alloc();
	p->k = CONS;
	p->u.cons.cdr = pop();
	p->u.cons.car = pop();
	push(p);
}

// returns 1 if expr p contains expr q, otherweise returns 0

int
find(struct atom *p, struct atom *q)
{
	int i;

	if (equal(p, q))
		return 1;

	if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (find(p->u.tensor->elem[i], q))
				return 1;
		return 0;
	}

	while (iscons(p)) {
		if (find(car(p), q))
			return 1;
		p = cdr(p);
	}

	return 0;
}

// Create a list from n things on the stack.

void
list(int n)
{
	int i;
	push_symbol(NIL);
	for (i = 0; i < n; i++)
		cons();
}

// Substitute new expr for old expr in expr.
//
// Input:	push	expr
//
//		push	old expr
//
//		push	new expr
//
// Output:	Result on stack

void
subst(void)
{
	int i;
	save();
	p3 = pop(); // new expr
	p2 = pop(); // old expr
	if (p2 == symbol(NIL) || p3 == symbol(NIL)) {
		restore();
		return;
	}
	p1 = pop(); // expr
	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		for (i = 0; i < p1->u.tensor->nelem; i++) {
			push(p1->u.tensor->elem[i]);
			push(p2);
			push(p3);
			subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
	} else if (equal(p1, p2))
		push(p3);
	else if (iscons(p1)) {
		push(car(p1));
		push(p2);
		push(p3);
		subst();
		push(cdr(p1));
		push(p2);
		push(p3);
		subst();
		cons();
	} else
		push(p1);
	restore();
}

int
length(struct atom *p)
{
	int n = 0;
	while (iscons(p)) {
		n++;
		p = cdr(p);
	}
	return n;
}

int
complexity(struct atom *p)
{
	int n = 1;

	while (iscons(p)) {
		n += complexity(car(p));
		p = cdr(p);
	}

	return n;
}

int
equal(struct atom *p1, struct atom *p2)
{
	double d;

	if (p1 == p2)
		return 1;

	if (isrational(p1) && isdouble(p2)) {
		push(p1);
		d = pop_double();
		if (d == p2->u.d)
			return 1;
		else
			return 0;
	}

	if (isdouble(p1) && isrational(p2)) {
		push(p2);
		d = pop_double();
		if (p1->u.d == d)
			return 1;
		else
			return 0;
	}

	if (p1->k != p2->k)
		return 0;

	switch (p1->k) {
	case CONS:
		while (iscons(p1) && iscons(p2)) {
			if (!equal(car(p1), car(p2)))
				return 0;
			p1 = cdr(p1);
			p2 = cdr(p2);
		}
		if (p1 == symbol(NIL) && p2 == symbol(NIL))
			return 1;
		else
			return 0;
	case KSYM:
		if (strcmp(p1->u.ksym.name, p2->u.ksym.name) == 0)
			return 1;
		else
			return 0;
	case USYM:
		if (strcmp(p1->u.usym.name, p2->u.usym.name) == 0)
			return 1;
		else
			return 0;
	case RATIONAL:
		if (p1->sign != p2->sign)
			return 0;
		if (!meq(p1->u.q.a, p2->u.q.a))
			return 0;
		if (!meq(p1->u.q.b, p2->u.q.b))
			return 0;
		return 1;
	case DOUBLE:
		if (p1->u.d == p2->u.d)
			return 1;
		else
			return 0;
	case STR:
		if (strcmp(p1->u.str, p2->u.str) == 0)
			return 1;
		else
			return 0;
	case TENSOR:
		return 0; // no need to sort tensors
	}
	return 0; // stop compiler warning
}

int
lessp(struct atom *p1, struct atom *p2)
{
	if (cmp_expr(p1, p2) < 0)
		return 1;
	else
		return 0;
}

void
sort(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_func);
}

int
sort_func(const void *q1, const void *q2)
{
	return cmp_expr(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_expr(struct atom *p1, struct atom *p2)
{
	int n;

	if (p1 == p2)
		return 0;

	if (p1 == symbol(NIL))
		return -1;

	if (p2 == symbol(NIL))
		return 1;

	if (isnum(p1) && isnum(p2))
		return compare_numbers(p1, p2);

	if (isnum(p1))
		return -1;

	if (isnum(p2))
		return 1;

	if (isstr(p1) && isstr(p2))
		return sign(strcmp(p1->u.str, p2->u.str));

	if (isstr(p1))
		return -1;

	if (isstr(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return sign(strcmp(printname(p1), printname(p2)));

	if (issymbol(p1))
		return -1;

	if (issymbol(p2))
		return 1;

	if (istensor(p1) && istensor(p2))
		return compare_tensors(p1, p2);

	if (istensor(p1))
		return -1;

	if (istensor(p2))
		return 1;

	while (iscons(p1) && iscons(p2)) {
		n = cmp_expr(car(p1), car(p2));
		if (n != 0)
			return n;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1;

	if (iscons(p1))
		return 1;

	return 0;
}

int
sign(int n)
{
	if (n < 0)
		return -1;
	if (n > 0)
		return 1;
	return 0;
}
