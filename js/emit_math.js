/* exported emit_math */

function
emit_math()
{
	var h, p, u, w, x, y;

	p = pop();
	u = emit_line(p, 0);

	outbuf = "";

	x = FONT_SIZE / 2;
	y = u.height;

	emit_svg(u, x, y);

	// emit_svg_bbox(x, y, u.height, u.depth, u.width);

	h = u.height + u.depth;
	w = u.width + FONT_SIZE;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";

	stdout.innerHTML += outbuf;
}
