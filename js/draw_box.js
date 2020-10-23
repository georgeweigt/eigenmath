function
draw_box()
{
	var x1 = 0;
	var x2 = DRAW_SIZE;

	var y1 = 0;
	var y2 = DRAW_SIZE;

	draw_line(x1, y1, x2, y1, 2); // top line
	draw_line(x1, y2, x2, y2, 2); // bottom line

	draw_line(x1, y1, x1, y2, 2); // left line
	draw_line(x2, y1, x2, y2, 2); // right line
}
