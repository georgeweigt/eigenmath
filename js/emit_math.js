/* exported emit_math */

function
emit_math()
{
	var h, w, p = pop();
	p = emit_line(p);
	outbuf = "";
	emit_svg(p, 0, p.height);
	h = p.height + p.depth;
	w = p.width + THIN_SPACE; // extra so serif not cut off

	// outbuf += "<rect x='0' y='0' height='" + h + "' width='" + w + "' stroke='black' stroke-width='1' fill='none'/>";

	stdout.innerHTML += "<p><svg height='" + h + "' width='" + w + "'>" + outbuf + "</svg></p>";
}
