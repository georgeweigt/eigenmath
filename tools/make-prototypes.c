#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int filter(const struct dirent *);
void scan(char *);
char *read_file(char *);

int
main(int argc, char *argv[])
{
	int i, len, n;
	char *filename;
	struct dirent **p;

	n = scandir(argv[1], &p, filter, alphasort);

	for (i = 0; i < n; i++) {

		len = strlen(argv[1]) + 1 + strlen(p[i]->d_name) + 1;

		filename = malloc(len);

		if (filename == NULL)
			exit(1);

		strcpy(filename, argv[1]);
		strcat(filename, "/");
		strcat(filename, p[i]->d_name);

		scan(filename);

		free(filename);
	}
}

int
filter(const struct dirent *p)
{
	int len = strlen(p->d_name);
	return len > 2 && strcmp(p->d_name + len - 2, ".c") == 0;
}

void
scan(char *filename)
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
