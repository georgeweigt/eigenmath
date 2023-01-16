char *
read_file(char *filename)
{
	int fd, n;
	char *buf;

	fd = open(filename, O_RDONLY, 0);

	if (fd == -1)
		return NULL;

	n = (int) lseek(fd, 0, SEEK_END);

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
