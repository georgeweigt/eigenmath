void
eval_status(struct atom *p1)
{
	(void) p1; // silence compiler

	outbuf_init();

	snprintf(strbuf, STRBUFLEN, "block_count %d (%d%%)\n", block_count, 100 * block_count / MAXBLOCKS);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "free_count %d\n", free_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "gc_count %d\n", gc_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "bignum_count %d\n", bignum_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "ksym_count %d\n", ksym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "usym_count %d\n", usym_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "string_count %d\n", string_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "tensor_count %d\n", tensor_count);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_eval_level %d\n", max_eval_level);
	outbuf_puts(strbuf);

	snprintf(strbuf, STRBUFLEN, "max_tos %d (%d%%)\n", max_tos, 100 * max_tos / STACKSIZE);
	outbuf_puts(strbuf);

	printbuf(outbuf, BLACK);

	push_symbol(NIL);
}
