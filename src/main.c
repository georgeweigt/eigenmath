#include "defs.h"

int html_flag;
int latex_flag;
char *infile;
char inbuf[1000];

int
main(int argc, char *argv[])
{
	int i;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--html") == 0)
			html_flag = 1;
		else if (strcmp(argv[i], "--latex") == 0)
			latex_flag = 1;
		else
			infile = argv[i];
	}

	clear();

	if (html_flag)
		printf("<html><head></head><body style='font-size:20pt'>\n\n");
	else if (latex_flag)
		begin_document();

	if (infile == NULL)
		for (;;)
			eval_stdin();

	run_infile();

	if (html_flag)
		printf("</body></html>\n");
	else if (latex_flag)
		end_document();

	return 0;
}

void
eval_stdin(void)
{
	if (html_flag)
		printf("<!--? ");
	else if (latex_flag)
		printf("%%? ");
	else
		printf("? ");

	fgets(inbuf, sizeof inbuf, stdin);

	if (html_flag)
		printf("-->\n");

	if (html_flag || latex_flag)
		printbuf(inbuf, BLUE);

	run(inbuf);
}

void
run_infile(void)
{
	int fd, n;
	char *buf;

	fd = open(infile, O_RDONLY, 0);

	if (fd == -1) {
		printf("cannot open %s\n", infile);
		exit(1);
	}

	// get file size

	n = lseek(fd, 0, SEEK_END);

	if (n == -1) {
		printf("lseek err\n");
		exit(1);
	}

	lseek(fd, 0, SEEK_SET);

	buf = malloc(n + 1);

	if (buf == NULL)
		malloc_kaput();

	if (read(fd, buf, n) != n) {
		printf("read err\n");
		exit(1);
	}

	close(fd);

	buf[n] = 0;
	run(buf);
	free(buf);
}

void
malloc_kaput(void)
{
	printf("malloc kaput\n");
	exit(1);
}

void
printbuf(char *s, int color)
{
	if (html_flag) {

		switch (color) {
		case BLACK:
			ffputs("<p style='color:black;font-family:courier'>\n");
			break;
		case BLUE:
			ffputs("<p style='color:blue;font-family:courier'>\n");
			break;
		case RED:
			ffputs("<p style='color:red;font-family:courier'>\n");
			break;
		default:
			ffputs("<p style='font-family:courier'>\n");
			break;
		}

		while (*s) {
			if (*s == '\n')
				ffputs("<br>\n");
			else if (*s == '&')
				ffputs("&amp;");
			else if (*s == '<')
				ffputs("&lt;");
			else if (*s == '>')
				ffputs("&gt;");
			else
				ffputc(*s);
			s++;
		}

		ffputc('\n');

	} else if (latex_flag) {

		ffputs("\\begin{verbatim}\n");
		ffputs(s);
		ffputs("\\end{verbatim}\n\n");

	} else
		ffputs(s);
}

void
ffputs(char *s)
{
	fputs(s, stdout);
}

void
ffputc(int c)
{
	fputc(c, stdout);
}

void
eval_draw(void)
{
	push_symbol(NIL);
}

void
cmdisplay(void)
{
	if (html_flag) {
		ffputs("<p>\n");
		mathml();
		ffputs(outbuf);
		ffputs("\n\n");
	} else if (latex_flag) {
		latex();
		ffputs(outbuf);
		ffputs("\n\n");
	} else
		display();
}

void
eval_exit(void)
{
	if (html_flag)
		printf("</body></html>\n");
	else if (latex_flag)
		end_document();

	exit(0);
}
