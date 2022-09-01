void
outbuf_init(void)
{
	outbuf_index = 0;
	outbuf_puts("");
}

void
outbuf_puts(char *s)
{
	int m, n;

	n = (int) strlen(s);

	// Let outbuf_index + n == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((outbuf_index + n) / 1000 + 1); // m is a multiple of 1000

	if (m > outbuf_length) {
		outbuf = realloc(outbuf, m);
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

	// Let outbuf_index + 1 == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((outbuf_index + 1) / 1000 + 1); // m is a multiple of 1000

	if (m > outbuf_length) {
		outbuf = realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}

	outbuf[outbuf_index++] = c;
	outbuf[outbuf_index] = '\0';
}
