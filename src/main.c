int
main(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++)
		run_infile(argv[i]);
	if (isatty(fileno(stdout)))
		run_stdin();
}

void
run_stdin(void)
{
	static char inbuf[1000];
	for (;;) {
		fputs("? ", stdout);
		fflush(stdout);
		fgets(inbuf, sizeof inbuf, stdin);
		run(inbuf);
	}
}

void
run_infile(char *infile)
{
	char *buf;
	buf = read_file(infile);
	if (buf == NULL) {
		fprintf(stderr, "cannot read %s\n", infile);
		exit(1);
	}
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
	fputs(outbuf, stdout);
	fputc('\n', stdout); // blank line after result
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
