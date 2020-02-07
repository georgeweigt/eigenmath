#include "defs.h"

int html_flag;
char *infile;

int
main(int argc, char *argv[])
{
	int i;
	static char buf[1000];

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--html") == 0)
			html_flag = 1;
		else
			infile = argv[i];
	}

	clear();

	if (infile) {
		run_infile();
		return 0;
	}

	for (;;) {
		printf("? ");
		fgets(buf, sizeof buf, stdin);
		run(buf);
	}

	return 0;
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

	if (html_flag)
		printf("<html><head></head><body style='font-size:20pt'>\n\n");

	buf[n] = 0;
	run(buf);
	free(buf);

	if (html_flag)
		printf("</body></html>\n");
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
			printstr("<p style='color:black;font-family:courier'>\n");
			break;
		case BLUE:
			printstr("<p style='color:blue;font-family:courier'>\n");
			break;
		case RED:
			printstr("<p style='color:red;font-family:courier'>\n");
			break;
		default:
			printstr("<p style='font-family:courier'>\n");
			break;
		}

		while (*s) {
			if (*s == '\n')
				printstr("<br>\n");
			else if (*s == '&')
				printstr("&amp;");
			else if (*s == '<')
				printstr("&lt;");
			else if (*s == '>')
				printstr("&gt;");
			else
				printchar(*s);
			s++;
		}

		printchar('\n');
	} else
		printstr(s);
}

void
printstr(char *s)
{
	fputs(s, stdout);
}

void
printchar(int c)
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
		printstr("<p>");
		mml();
		printstr(outbuf);
		printstr("\n\n");
	} else
		display();
}
