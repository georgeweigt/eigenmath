void
eval_run(struct atom *p1)
{
	char *buf;
	struct atom *p2;

	push(cadr(p1));
	evalf();
	p1 = pop();
	if (!isstr(p1))
		stopf("run: file name expected");

	p2 = alloc_str();
	buf = read_file(p1->u.str);
	if (buf == NULL)
		stopf("run: cannot read file");
	p2->u.str = buf;

	fpush(p2); // make visible to garbage collector
	run_buf(buf);
	fpop(); // buf is freed on next gc

	push_symbol(NIL); // return value
}
