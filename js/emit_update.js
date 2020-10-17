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

	u.height = h;
	u.depth = d;
	u.width = w;
}
