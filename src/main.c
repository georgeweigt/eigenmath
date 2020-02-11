#include "defs.h"

int html_flag;
int html_state;
int latex_flag;
int latex_state;
int mathjax_flag;
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
		else if (strcmp(argv[i], "--mathjax") == 0)
			mathjax_flag = 1;
		else
			infile = argv[i];
	}

	clear();

	begin_document();

	if (infile == NULL)
		for (;;)
			eval_stdin();

	run_infile();

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
printbuf(char *s, int color)
{
	if (html_flag || mathjax_flag) {

		switch (color) {

		case BLACK:
			if (html_state != 1) {
				fputs("<p style='color:black;font-family:courier'>\n", stdout);
				html_state = 1;
			}
			break;

		case BLUE:
			if (html_state != 2) {
				fputs("<p style='color:blue;font-family:courier'>\n", stdout);
				html_state = 2;
			}
			break;

		case RED:
			if (html_state != 3) {
				fputs("<p style='color:red;font-family:courier'>\n", stdout);
				html_state = 3;
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

	} else if (latex_flag) {

		if (latex_state == 0) {
			fputs("\\begin{verbatim}\n", stdout);
			latex_state = 1;
		}

		fputs(s, stdout);

	} else
		fputs(s, stdout);
}

void
cmdisplay(void)
{
	if (html_flag) {

		mathml();

		fputs("<p>\n", stdout);
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		html_state = 0;

	} else if (latex_flag) {

		latex();

		if (latex_state)
			fputs("\\end{verbatim}\n\n", stdout);
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		latex_state = 0;

	} else if (mathjax_flag) {

		mathjax();

		fputs("<p>\n", stdout);
		fputs(outbuf, stdout);
		fputs("\n\n", stdout);

		html_state = 0;

	} else
		display();
}

void
begin_document(void)
{
	if (html_flag)
		begin_html();
	else if (latex_flag)
		begin_latex();
	else if (mathjax_flag)
		begin_mathjax();
}

void
end_document(void)
{
	if (html_flag)
		end_html();
	else if (latex_flag)
		end_latex();
	else if (mathjax_flag)
		end_mathjax();
}

void
begin_html(void)
{
	fputs("<html>\n<head>\n</head>\n<body style='font-size:20pt'>\n\n", stdout);
}

void
end_html(void)
{
	fputs("</body>\n</html>\n", stdout);
}

void
begin_latex(void)
{
	fputs(
	"\\documentclass[12pt]{article}\n"
	"\\usepackage{amsmath,amsfonts,amssymb}\n"
	"\% change margins\n"
	"\\addtolength{\\oddsidemargin}{-.875in}\n"
	"\\addtolength{\\evensidemargin}{-.875in}\n"
	"\\addtolength{\\textwidth}{1.75in}\n"
	"\\addtolength{\\topmargin}{-.875in}\n"
	"\\addtolength{\\textheight}{1.75in}\n"
	"\\begin{document}\n\n",
	stdout);
}

void
end_latex(void)
{
	if (latex_state)
		fputs("\\end{verbatim}\n\n", stdout);
	fputs("\\end{document}\n", stdout);
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
	"<body>\n",
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
