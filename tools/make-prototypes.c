#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
void emit(char *);
char *read_file(char *);

int
main(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++)
		emit(argv[i]);
}

void
emit(char *filename)
{
	int i, j, k;
	char *buf, *s;

	buf = read_file(filename);

	if (buf == NULL) {
		fprintf(stderr, "cannot read %s\n", filename);
		exit(1);
	}

	i = 0;

	for (;;) {

		// find start of function block

		s = strstr(buf + i, "\n{");

		if (s == NULL)
			break;

		k = (int) (s - buf);

		// go back 2 newlines

		j = k;

		while (j > 0 && buf[j - 1] != '\n')
			j--;

		if (j > 0)
			j--;

		while (j > 0 && buf[j - 1] != '\n')
			j--;

		// print

		for (i = j; i < k; i++)
			if (buf[i] == '\n')
				putchar(' ');
			else
				putchar(buf[i]);

		putchar(';');
		putchar('\n');

		i = k + 2;
	}

	free(buf);
}

char *
read_file(char *filename)
{
	int fd, n;
	char *buf;

	fd = open(filename, O_RDONLY, 0);

	if (fd == -1)
		return NULL;

	n = lseek(fd, 0, SEEK_END);

	if (n == -1) {
		close(fd);
		return NULL;
	}

	if (lseek(fd, 0, SEEK_SET) == -1) {
		close(fd);
		return NULL;
	}

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		return NULL;
	}

	if (read(fd, buf, n) != n) {
		close(fd);
		free(buf);
		return NULL;
	}

	close(fd);

	buf[n] = '\0';

	return buf;
}
