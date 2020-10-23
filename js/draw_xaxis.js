function
draw_xaxis()
{
	var y = DRAW_HEIGHT * (1 + ymin / (ymax - ymin));
	if (y > 0 && y < DRAW_HEIGHT)
		draw_line(0, y, DRAW_WIDTH, y, 0.5);
}
