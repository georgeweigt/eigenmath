/* exported emit_svg_bbox */

function
emit_svg_bbox(x, y, h, d, w)
{
	emit_svg_line(x, y - h, x + w, y - h, 1);	// top
	emit_svg_line(x, y + d, x + w, y + d, 1);	// bottom
	emit_svg_line(x, y - h, x, y + d, 1);		// left
	emit_svg_line(x + w, y - h, x + w, y + d, 1);	// right
}
