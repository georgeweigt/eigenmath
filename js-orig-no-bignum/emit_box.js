function
emit_box()
{
	var x1 = 0;
	var x2 = DRAW_WIDTH;

	var y1 = 0;
	var y2 = DRAW_HEIGHT;

	draw_line(x1, y1, x2, y1, 0.5); // top line
	draw_line(x1, y2, x2, y2, 0.5); // bottom line

	draw_line(x1, y1, x1, y2, 0.5); // left line
	draw_line(x2, y1, x2, y2, 0.5); // right line
}
