void
outbuf_init(void)
{
	outbuf_index = 0;
	outbuf_puts(""); // so that string length is zero
}

void
outbuf_puts(char *s)
{
	int len, m;

	len = (int) strlen(s);

	// Let outbuf_index + len == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((outbuf_index + len) / 1000 + 1); // m is a multiple of 1000

	if (m > outbuf_length) {
		outbuf = realloc(outbuf, m);
		if (outbuf == NULL)
			exit(1);
		outbuf_length = m;
	}

	strcpy(outbuf + outbuf_index, s);
	outbuf_index += len;
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
