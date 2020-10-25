function
emit_delim_width(small_font)
{
	var size;

	if (small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	return symbol_width["&parenleft;"] * WIDTH_RATIO * size;
}
