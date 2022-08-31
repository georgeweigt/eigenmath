int
main(int argc, char *argv[])
{
	if (argc > 1)
		run_infile(argv[1]);

	if (isatty(fileno(stdout)))
		run_stdin();

	return 0;
}

void
run_stdin(void)
{
	static char inbuf[1000];
	for (;;) {
		printf("? ");
		fgets(inbuf, sizeof inbuf, stdin);
		run(inbuf);
	}
}

void
run_infile(char *infile)
{
	int fd, n;
	char *buf;

	fd = open(infile, O_RDONLY, 0);

	if (fd == -1) {
		fprintf(stderr, "cannot open %s\n", infile);
		exit(1);
	}

	// get file size

	n = lseek(fd, 0, SEEK_END);

	if (n == -1) {
		fprintf(stderr, "lseek err\n");
		exit(1);
	}

	lseek(fd, 0, SEEK_SET);

	buf = malloc(n + 1);

	if (buf == NULL)
		exit(1);

	if (read(fd, buf, n) != n) {
		fprintf(stderr, "read err\n");
		exit(1);
	}

	close(fd);

	buf[n] = '\0';
	run(buf);
	free(buf);
}

void
printbuf(char *s, int color)
{
	fputs(s, stdout);
}

void
display(void)
{
	fmt();
}

void
eval_draw(struct atom *p1)
{
	(void) p1; // silence compiler
	push_symbol(NIL);
}

void
eval_exit(struct atom *p1)
{
	(void) p1; // silence compiler
	exit(0);
}
