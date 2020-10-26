/* exported emit_math */

function
emit_math()
{
	var h, p, u, w;
	p = pop();
	u = emit_line(p, 0);
	outbuf = "";
	emit_svg(u, FONT_SIZE / 2, u.height);
	h = u.height + u.depth;
	w = u.width + FONT_SIZE;
	h = "height='" + h + "'";
	w = "width='" + w + "'";
	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";
	stdout.innerHTML += outbuf;
}
