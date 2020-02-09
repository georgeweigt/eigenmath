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
			fputs("<p style='color:black;font-family:courier'>\n", stdout);
			break;
		case BLUE:
			fputs("<p style='color:blue;font-family:courier'>\n", stdout);
			break;
		case RED:
			fputs("<p style='color:red;font-family:courier'>\n", stdout);
			break;
		default:
			fputs("<p style='font-family:courier'>\n", stdout);
			break;
		}

		while (*s) {
			if (*s == '\n')
				fputs("<br>\n", stdout);
			else if (*s == '&')
				fputs("&amp;", stdout);
			else if (*s == '<')
				fputs("&lt;", stdout);
			else if (*s == '>')
				fputs("&gt;", stdout);
			else
				fputc(*s, stdout);
			s++;
		}

		fputc('\n', stdout);

	} else if (latex_flag) {

		fputs("\\begin{verbatim}\n", stdout);
		fputs(s, stdout);
		fputs("\\end{verbatim}\n\n", stdout);

	} else
		fputs(s, stdout);
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
		fputs("<p>\n", stdout);
		mathml();
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);
	} else if (latex_flag) {
		latex();
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);
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

char *begin_document_str =
"\\documentclass[12pt]{article}\n"
"\\usepackage{amsmath,amsfonts,amssymb}\n"
"\% change margins\n"
"\\addtolength{\\oddsidemargin}{-.875in}\n"
"\\addtolength{\\evensidemargin}{-.875in}\n"
"\\addtolength{\\textwidth}{1.75in}\n"
"\\addtolength{\\topmargin}{-.875in}\n"
"\\addtolength{\\textheight}{1.75in}\n"
"\\begin{document}\n\n";

char *end_document_str = "\\end{document}\n";

void
begin_document(void)
{
	fputs(begin_document_str, stdout);
}

void
end_document(void)
{
	fputs(end_document_str, stdout);
}
