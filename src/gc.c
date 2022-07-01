#include "defs.h"

// can only be called from main run loop

void
gc(void)
{
	int i, j, k;
	struct atom *p;

	if (level)
		kaput("internal error 2");

	gc_count++;

	// tag everything

	for (i = 0; i < block_count; i++) {
		p = mem[i];
		for (j = 0; j < BLOCKSIZE; j++)
			p[j].tag = 1;
	}

	// untag what's used

	untag(zero);
	untag(one);
	untag(minusone);
	untag(imaginaryunit);

	// symbol table

	for (i = 0; i < 27; i++) {
		for (j = 0; j < NSYM; j++) {
			k = NSYM * i + j;
			if (symtab[k] == NULL)
				break;
			untag(symtab[k]);
			untag(binding[k]);
			untag(usrfunc[k]);
		}
	}

	// collect everything that's still tagged

	free_list = NULL;
	free_count = 0;

	for (i = 0; i < block_count; i++) {

		p = mem[i];

		for (j = 0; j < BLOCKSIZE; j++) {

			if (p[j].tag == 0)
				continue;

			// still tagged so it's unused, put on free list

			switch (p[j].atomtype) {
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
			default:
				break; // FREEATOM, CONS, or DOUBLE
			}

			p[j].atomtype = FREEATOM;
			p[j].u.next = free_list;

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
		return;

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
