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
	int i, n;
	struct dirent **p;
	char filename[100];

	n = scandir(argv[1], &p, filter, alphasort);

	for (i = 0; i < n; i++) {
		strcpy(filename, argv[1]);
		strcat(filename, "/");
		strcat(filename, p[i]->d_name);
		scan(filename);
	}
}

int
filter(const struct dirent *p)
{
	int len = strlen(p->d_name);

	if (len > 2 && strcmp(p->d_name + len - 2, ".c") == 0)
		return 1;
	else
		return 0;
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

		j = k - 1;

		while (j > -1 && buf[j] != '\n')
			j--;

		j--;

		while (j > -1 && buf[j] != '\n')
			j--;

		// print

		for (i = j + 1; i < k; i++)
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

	if (n < 0) {
		close(fd);
		return NULL;
	}

	if (lseek(fd, 0, SEEK_SET)) {
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
