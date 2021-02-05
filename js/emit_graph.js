function
emit_graph()
{
	var h, w;

	h = DRAW_TOP_PAD + DRAW_HEIGHT + DRAW_BOTTOM_PAD;
	w = DRAW_LEFT_PAD + DRAW_WIDTH + DRAW_RIGHT_PAD;

	h = "height='" + h + "'";
	w = "width='" + w + "'";

	outbuf = "<svg " + h + w + ">"

	draw_axes();
	draw_box();
	draw_labels();
	draw_points();

	outbuf += "</svg><br>";

	stdout.innerHTML += outbuf;
}
