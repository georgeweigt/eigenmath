/* exported emit_math */

function
emit_math()
{
	var h, p, u, w;
	p = pop();
	u = emit_line(p, 0);
	outbuf = "";
	emit_svg(u, FONT_SIZE / 2, u.height);
/*
	// bounding box

	var x = 0;
	var y = 0;

	var x2 = u.width + FONT_SIZE;
	var y2 = u.height + u.depth;

	h = y2 - y;
	w = x2 - x;

	x = "x='" + x + "'";
	y = "y='" + y + "'";

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf += "<rect " + x + y + h + w + "style='fill:none;stroke:black;stroke-width:1'/>";
*/
	h = u.height + u.depth;
	w = u.width + FONT_SIZE;
	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<p><svg " + h + w + ">\n" + outbuf + "</svg></p>";
	stdout.innerHTML += outbuf;
}
