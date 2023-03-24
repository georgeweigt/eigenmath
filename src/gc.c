// All struct atom pointers must be visible to the garbage collector.

// Specifically, any automatic struct atom pointers must also be on the stack when gc() is called.

// The condition loop_level == eval_level indicates that any automatic struct atom pointers are also on the stack.

void
gc_check(void)
{
	if (loop_level == eval_level && alloc_count > MAXBLOCKS * BLOCKSIZE / 10) {
		gc();
		alloc_count = 0;
	}
}

void
gc(void)
{
	int i, j, k;
	struct atom *p;

	gc_count++;

	// tag everything

	for (i = 0; i < block_count; i++)
		for (j = 0; j < BLOCKSIZE; j++)
			mem[i][j].tag = 1;

	// untag what's used

	untag(zero);
	untag(one);
	untag(minusone);
	untag(imaginaryunit);

	for (i = 0; i < tos; i++)
		untag(stack[i]);

	for (i = 0; i < tof; i++)
		untag(frame[i]);

	for (i = 0; i < toj; i++)
		untag(journal[i]);

	for (i = 0; i < 27; i++)
		for (j = 0; j < NSYM; j++) {
			k = NSYM * i + j;
			if (symtab[k] == NULL)
				break;
			untag(symtab[k]);
			untag(binding[k]);
			untag(usrfunc[k]);
		}

	// collect everything that's still tagged

	free_list = NULL;
	free_count = 0;

	for (i = 0; i < block_count; i++)
		for (j = 0; j < BLOCKSIZE; j++) {

			p = mem[i] + j;

			if (p->tag == 0)
				continue;

			// still tagged so it's unused, put on free list

			switch (p->atomtype) {
			case KSYM:
				free(p->u.ksym.name);
				ksym_count--;
				break;
			case USYM:
				free(p->u.usym.name);
				usym_count--;
				break;
			case RATIONAL:
				mfree(p->u.q.a);
				mfree(p->u.q.b);
				break;
			case STR:
				if (p->u.str)
					free(p->u.str);
				string_count--;
				break;
			case TENSOR:
				free(p->u.tensor);
				tensor_count--;
				break;
			default:
				break; // FREEATOM, CONS, or DOUBLE
			}

			p->atomtype = FREEATOM;
			p->u.next = free_list;

			free_list = p;
			free_count++;
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
