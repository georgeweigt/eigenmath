function
emit_svg_line(x1, y1, x2, y2, t)
{
	var X1 = " x1='" + x1 + "'";
	var Y1 = " y1='" + y1 + "'";
	var X2 = " x2='" + x2 + "'";
	var Y2 = " y2='" + y2 + "'";
	var T = " style='stroke:black;stroke-width:" + t + "'";

	outbuf += "<line" + X1 + Y1 + X2 + Y2 + T + "/>";
}
