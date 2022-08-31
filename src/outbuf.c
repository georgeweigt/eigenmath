void
outbuf_init(void)
{
	outbuf_index = 0;
	outbuf_putc('\n');
	outbuf_index = 0;
}

void
outbuf_puts(char *s)
{
	int m, n;
	n = (int) strlen(s);
	m = 1000 * ((outbuf_index + n) / 1000 + 1); // m is a multiple of 1000
	if (m > outbuf_length) {
		outbuf = (char *) realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}
	strcpy(outbuf + outbuf_index, s);
	outbuf_index += n;
}

void
outbuf_putc(int c)
{
	int m;
	m = 1000 * ((outbuf_index + 1) / 1000 + 1); // m is a multiple of 1000
	if (m > outbuf_length) {
		outbuf = (char *) realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}
	outbuf[outbuf_index++] = c;
	outbuf[outbuf_index] = '\0';
}
