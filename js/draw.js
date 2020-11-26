function
draw(F, X)
{
	var h, w;

	draw_array = [];

	h = DRAW_TOP_PAD + DRAW_HEIGHT + DRAW_BOTTOM_PAD;
	w = DRAW_LEFT_PAD + DRAW_WIDTH + DRAW_RIGHT_PAD;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<p><svg " + h + w + ">\n"

	draw_xrange();
	draw_yrange();
	draw_axes();
	draw_box();
	draw_labels();
	draw_pass1(F, X);
	draw_pass2(F, X);
	draw_points();

	outbuf += "</svg></p>\n";

	stdout.innerHTML += outbuf;
}
