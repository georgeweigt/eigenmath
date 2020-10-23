/* exported emit_math */

function
emit_math()
{
	var h, p, w;
	p = pop();
	p = emit_line(p, 0);
	outbuf = "";
	emit_svg(p, FONT_SIZE / 2, p.height);
	h = p.height + p.depth;
	w = p.width + FONT_SIZE;
	h = "height='" + h + "'";
	w = "width='" + w + "'";
	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";
	stdout.innerHTML += outbuf;
}
