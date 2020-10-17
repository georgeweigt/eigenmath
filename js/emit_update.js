function
emit_update(u)
{
	var d, h, i, w;

	h = 0;
	d = 0;
	w = 0;

	for (i = 0; i < u.a.length; i++) {
		h = Math.max(h, u.a[i].height);
		d = Math.max(d, u.a[i].depth);
		w += u.a[i].width;
	}

	if (u.type == SUPERSCRIPT) {
		h = h + d + X_HEIGHT;
		d = 0;
	}

	if (u.type == SUBSCRIPT) {
		h -= X_HEIGHT;
		d += X_HEIGHT;
	}

	if (u.type == PAREN || u.type == BRACK)
		w += 2 * FONT_WIDTH;

	u.height = h;
	u.depth = d;
	u.width = w;
}
