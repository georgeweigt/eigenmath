/* exported emit_math */

function
emit_math()
{
	var h, p, u, w, x, y;

	p = pop();
	u = emit_line(p, 0);

	outbuf = "";

	x = HPAD;
	y = u.height + VPAD;

	emit_svg(u, x, y);

	// emit_svg_bbox(x, y, u.height, u.depth, u.width);

	h = u.height + u.depth + 2 * VPAD;
	w = u.width + 2 * HPAD;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";

	stdout.innerHTML += outbuf;
}
