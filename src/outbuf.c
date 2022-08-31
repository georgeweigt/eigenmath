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
	int n;
	n = (int) strlen(s);
	if (outbuf_index + n + 1 > outbuf_length) {
		outbuf_length = 1000 * ((outbuf_index + n) / 1000 + 1); // length is multiple of 1000
		outbuf = (char *) realloc(outbuf, outbuf_length);
		if (outbuf == NULL)
			exit(1);
	}
	strcpy(outbuf + outbuf_index, s);
	outbuf_index += n;
}

void
outbuf_putc(int c)
{
	if (outbuf_index + 2 > outbuf_length) {
		outbuf_length += 1000;
		outbuf = (char *) realloc(outbuf, outbuf_length);
		if (outbuf == NULL)
			exit(1);
	}
	outbuf[outbuf_index++] = c;
	outbuf[outbuf_index] = '\0';
}
