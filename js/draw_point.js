function
draw_point(F, X, x, pass1)
{
	var cx, cy, dx, dy, p, y;

	draw_evalf(F, X, x);
	p = pop();
	if (!isnum(p))
		return;
	push(p);
	y = pop_double();

	dx = DRAW_WIDTH * (x - xmin) / (xmax - xmin);
	dy = DRAW_HEIGHT * (y - ymin) / (ymax - ymin);

	dy = DRAW_HEIGHT - dy; // flip

	if (dy >= 0 && dy <= DRAW_HEIGHT) {

		cx = dx + DRAW_LEFT_PAD;
		cy = dy + DRAW_TOP_PAD;

		cx = "cx='" + cx + "'";
		cy = "cy='" + cy + "'";

		outbuf += "<circle " + cx + " " + cy + " r='2' style='stroke:black;fill:black'/>";
	}

	if (pass1)
		draw_array.push({x:x, y:y, dx:dx, dy:dy});
}
