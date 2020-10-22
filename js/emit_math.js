/* exported emit_math */

function
emit_math()
{
	var h, w, p = pop();
	p = emit_line(p);
	outbuf = "";
	emit_svg(p, FONT_SIZE / 2, p.height);
	h = p.height + p.depth;
	w = p.width + FONT_SIZE;
	h = "height='" + h + "'";
	w = "width='" + w + "'";
	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";
	//print_buf(outbuf, RED);
	stdout.innerHTML += outbuf;
}
