function
emit_delim_width(small)
{
	var size;

	if (small)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	return glyph_info["&parenleft;"].width * WIDTH_RATIO * size;
}
