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

char *
read_file(char *filename)
{
	int fd, n;
	char *buf;
	off_t t;

	fd = open(filename, O_RDONLY);

	if (fd < 0)
		return NULL;

	t = lseek(fd, 0, SEEK_END);

	if (t < 0 || t > 0x1000000) { // 16 MB max
		close(fd);
		return NULL;
	}

	if (lseek(fd, 0, SEEK_SET)) {
		close(fd);
		return NULL;
	}

	n = (int) t;

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		return NULL;
	}

	if (read(fd, buf, n) != n) {
		free(buf);
		close(fd);
		return NULL;
	}

	close(fd);

	buf[n] = '\0';

	return buf;
}
