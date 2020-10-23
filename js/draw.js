function
draw(F, X)
{
	draw_xrange();
	draw_yrange();

	var h = DRAW_SIZE + 4 * DRAW_PAD;
	var w = DRAW_INDENT + DRAW_SIZE + 4 * DRAW_PAD;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<p><svg " + h + w + ">\n"

	draw_xaxis();
	draw_yaxis();
	draw_box();
	draw_labels();

	draw_array = [];

	draw_data(F, X);
	draw_fill(F, X);

	outbuf += "</svg></p>\n";

	stdout.innerHTML += outbuf;
}
