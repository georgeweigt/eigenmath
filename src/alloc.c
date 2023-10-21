struct atom *
alloc_atom(void)
{
	struct atom *p;

	if (free_count == 0)
		alloc_block();

	p = free_list;
	free_list = p->u.next;

	free_count--;
	alloc_count++;

	return p;
}

void
alloc_block(void)
{
	int i;
	struct atom *p;

	if (block_count == MAXBLOCKS) {
		tos = 0;
		tof = 0;
		gc(); // prep for next run
		exitf("out of memory");
	}

	p = alloc_mem(BLOCKSIZE * sizeof (struct atom));

	mem[block_count++] = p;

	for (i = 0; i < BLOCKSIZE - 1; i++) {
		p[i].atomtype = FREEATOM;
		p[i].u.next = p + i + 1;
	}

	p[i].atomtype = FREEATOM;
	p[i].u.next = NULL;

	free_list = p;
	free_count = BLOCKSIZE;
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
	struct atom *p;
	struct tensor *t;
	p = alloc_atom();
	t = alloc_mem(sizeof (struct tensor) + nelem * sizeof (struct atom *));
	p->atomtype = TENSOR;
	p->u.tensor = t;
	t->nelem = nelem;
	for (i = 0; i < nelem; i++)
		t->elem[i] = zero;
	tensor_count++;
	return p;
}

struct atom *
alloc_str(void)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = STR;
	p->u.str = NULL;
	string_count++;
	return p;
}

void *
alloc_mem(int n)
{
	void *p = malloc(n);
	if (p == NULL)
		exit(1);
	return p;
}
