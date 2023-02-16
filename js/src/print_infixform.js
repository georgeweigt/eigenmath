function
print_infixform(p)
{
	outbuf = "";
	infixform_expr(p);
	infixform_write("\n");
	printbuf(outbuf, BLACK);
}
