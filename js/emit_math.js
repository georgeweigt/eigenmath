/* exported emit_math */

function
emit_math()
{
	var h, w, p = pop();
	p = emit_line(p);
	outbuf = "";
	emit_svg(p, 0, p.height);
	h = p.height + p.depth;
	w = p.width + FONT_WIDTH; // extra so serif not cut off

//print_buf("<p><svg height='" + h + "' width='" + w + "'>" + outbuf + "</svg></p>", RED);

	stdout.innerHTML += "<p><svg height='" + h + "' width='" + w + "'>" + outbuf + "</svg></p>";
}
