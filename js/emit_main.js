function
emit_main(p)
{
	var i, n, u;
	u = {type:HARRAY, a:[], height:0, width:0};
	emit_expr(u, p);
	n = u.a.length;
	for (i = 0; i < n; i++) {
		u.height = Math.max(u.height, u.a[i].height);
		u.width += u.a[i].width;
	}
	return u;
}
