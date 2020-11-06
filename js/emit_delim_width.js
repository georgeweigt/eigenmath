function
emit_delim_width(u)
{
	var size;

	if (u.level == 0)
		size = FONT_SIZE;
	else
		size = SMALL_FONT_SIZE;

	return glyph_info["&parenleft;"].width * WIDTH_RATIO * size;
}
