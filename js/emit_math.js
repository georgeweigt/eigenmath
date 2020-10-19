/* exported emit_math */

function
emit_math()
{
	var h, w, p = pop();
	p = emit_line(p);
	outbuf = "";
	emit_svg(p, 0, 0);
	h = p.height + p.depth;
	w = p.width;
	stdout.innerHTML += "<p><svg height='" + h + "' width='" + w + "'>" + outbuf + "</svg></p>";
}
