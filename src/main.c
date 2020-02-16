#include "defs.h"

int doc_type;
int doc_state;
char *infile;
char inbuf[1000];

#define DOC_LATEX 1
#define DOC_MATHML 2
#define DOC_MATHJAX 3

int
main(int argc, char *argv[])
{
	int i;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--latex") == 0)
			doc_type = DOC_LATEX;
		else if (strcmp(argv[i], "--mathml") == 0)
			doc_type = DOC_MATHML;
		else if (strcmp(argv[i], "--mathjax") == 0)
			doc_type = DOC_MATHJAX;
		else
			infile = argv[i];
	}

	begin_document();

	if (infile)
		run_infile();

	if (isatty(fileno(stdout)))
		run_stdin();

	end_document();

	return 0;
}

void
run_stdin(void)
{
	for (;;) {
		prompt();
		fgets(inbuf, sizeof inbuf, stdin);
		echo();
		run(inbuf);
	}
}

void
prompt(void)
{
	switch (doc_type) {

	case DOC_LATEX:
		printf("%%? ");
		break;

	case DOC_MATHML:
	case DOC_MATHJAX:
		printf("<!--? ");
		break;

	default:
		printf("? ");
		break;
	}
}

void
echo(void)
{
	switch (doc_type) {

	case DOC_LATEX:
		printbuf(inbuf, BLUE);
		break;

	case DOC_MATHML:
	case DOC_MATHJAX:
		printf("-->\n");
		printbuf(inbuf, BLUE);
		break;

	default:
		break;
	}
}

void
run_infile(void)
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
		malloc_kaput();

	if (read(fd, buf, n) != n) {
		fprintf(stderr, "read err\n");
		exit(1);
	}

	close(fd);

	buf[n] = 0;
	run(buf);
	free(buf);
}

void
printbuf(char *s, int color)
{
	switch (doc_type) {

	case DOC_LATEX:

		if (doc_state == 0) {
			fputs("\\begin{verbatim}\n", stdout);
			doc_state = 1;
		}

		fputs(s, stdout);

		break;

	case DOC_MATHML:
	case DOC_MATHJAX:

		switch (color) {

		case BLACK:
			if (doc_state != 1) {
				fputs("<p style='color:black;font-family:courier;font-size:20pt'>\n", stdout);
				doc_state = 1;
			}
			break;

		case BLUE:
			if (doc_state != 2) {
				fputs("<p style='color:blue;font-family:courier;font-size:20pt'>\n", stdout);
				doc_state = 2;
			}
			break;

		case RED:
			if (doc_state != 3) {
				fputs("<p style='color:red;font-family:courier;font-size:20pt'>\n", stdout);
				doc_state = 3;
			}
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

		break;

	default:
		fputs(s, stdout);
		break;
	}
}

void
cmdisplay(void)
{
	switch (doc_type) {

	case DOC_LATEX:

		latex();

		if (doc_state)
			fputs("\\end{verbatim}\n\n", stdout);

		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		break;

	case DOC_MATHML:

		mathml();

		fputs("<p style='font-size:20pt'>\n", stdout);
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		break;

	case DOC_MATHJAX:

		mathjax();

		fputs("<p style='font-size:20pt'>\n", stdout);
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		break;

	default:
		display();
		break;
	}

	doc_state = 0;
}

void
begin_document(void)
{
	switch (doc_type) {

	case DOC_LATEX:
		begin_latex();
		break;

	case DOC_MATHML:
		begin_mathml();
		break;

	case DOC_MATHJAX:
		begin_mathjax();
		break;

	default:
		break;
	}
}

void
end_document(void)
{
	switch (doc_type) {

	case DOC_LATEX:
		end_latex();
		break;

	case DOC_MATHML:
		end_mathml();
		break;

	case DOC_MATHJAX:
		end_mathjax();
		break;

	default:
		break;
	}
}

void
begin_latex(void)
{
	fputs(
	"\\documentclass[12pt]{article}\n"
	"\\usepackage{amsmath,amsfonts,amssymb}\n"
	"\\usepackage[margin=2cm]{geometry}\n"
	"\\begin{document}\n\n",
	stdout);
}

void
end_latex(void)
{
	if (doc_state)
		fputs("\\end{verbatim}\n\n", stdout);
	fputs("\\end{document}\n", stdout);
}

void
begin_mathml(void)
{
	fputs("<html>\n<head>\n</head>\n<body>\n\n", stdout);
}

void
end_mathml(void)
{
	fputs("</body>\n</html>\n", stdout);
}

void
begin_mathjax(void)
{
	fputs(
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<head>\n"
	"<script src='https://polyfill.io/v3/polyfill.min.js?features=es6'></script>\n"
	"<script type='text/javascript' id='MathJax-script' async\n"
	"src='https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-chtml.js'></script>\n"
	"</head>\n"
	"<body>\n\n",
	stdout);
}

void
end_mathjax(void)
{
	fputs("</body>\n</html>\n", stdout);
}

void
eval_draw(void)
{
	push_symbol(NIL);
}

void
eval_exit(void)
{
	end_document();
	exit(0);
}

void
malloc_kaput(void)
{
	fprintf(stderr, "malloc kaput\n");
	exit(1);
}
