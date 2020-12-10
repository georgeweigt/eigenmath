const THICK_DELIM = 2.5;
const MEDIUM_DELIM = 2.2;
const THIN_DELIM = 1.5;

const FRACTION_STROKE = 2.0;
const SMALL_FRACTION_STROKE = 1.5;

function
draw_formula(x, y, p)
{
	var char_num, d, dx, dy, font_num, h, k, stroke_width, w;

	k = opcode(p);
	h = height(p);
	d = depth(p);
	w = width(p);

	p = cddddr(p);

	switch (k) {

	case EMIT_SPACE:
		break;

	case EMIT_CHAR:
		font_num = val1(p);
		char_num = val2(p);
		draw_char(x, y, font_num, char_num);
		break;

	case EMIT_LIST:
		p = car(p);
		while (iscons(p)) {
			draw_formula(x, y, car(p));
			x += width(car(p));
			p = cdr(p);
		}
		break;

	case EMIT_SUPERSCRIPT:
	case EMIT_SUBSCRIPT:
		dx = val1(p);
		dy = val2(p);
		p = caddr(p);
		draw_formula(x + dx, y + dy, p);
		break;

	case EMIT_SUBEXPR:
		draw_delims(x, y, h, d, w, MEDIUM_DELIM, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, LEFT_PAREN);
		draw_formula(x + dx, y, car(p));
		break;

	case EMIT_SMALL_SUBEXPR:
		draw_delims(x, y, h, d, w, THIN_DELIM, SMALL_ROMAN_FONT);
		dx = get_char_width(SMALL_ROMAN_FONT, LEFT_PAREN);
		draw_formula(x + dx, y, car(p));
		break;

	case EMIT_FRACTION:
	case EMIT_SMALL_FRACTION:

		// horizontal line

		if (k == EMIT_FRACTION) {
			dy = get_operator_height(ROMAN_FONT);
			stroke_width = FRACTION_STROKE;
		} else {
			dy = get_operator_height(SMALL_ROMAN_FONT);
			stroke_width = SMALL_FRACTION_STROKE;
		}

		draw_stroke(x, y - dy, x + w, y - dy, stroke_width);

		// numerator

		dx = (w - width(car(p))) / 2;
		dy = -h + height(car(p));
		draw_formula(x + dx, y + dy, car(p));

		// denominator

		p = cdr(p);
		dx = (w - width(car(p))) / 2;
		dy = d - depth(car(p));
		draw_formula(x + dx, y + dy, car(p));

		break;

	case EMIT_TABLE:
		draw_delims(x, y, h, d, w, THICK_DELIM, ROMAN_FONT);
		dx = get_char_width(ROMAN_FONT, LEFT_PAREN);
		draw_table(x + dx, y - h, p);
		break;
	}
}

const html_name_tab = [

	"&Alpha;",
	"&Beta;",
	"&Gamma;",
	"&Delta;",
	"&Epsilon;",
	"&Zeta;",
	"&Eta;",
	"&Theta;",
	"&Iota;",
	"&Kappa;",
	"&Lambda;",
	"&Mu;",
	"&Nu;",
	"&Xi;",
	"&Omicron;",
	"&Pi;",
	"&Rho;",
	"&Sigma;",
	"&Tau;",
	"&Upsilon;",
	"&Phi;",
	"&Chi;",
	"&Psi;",
	"&Omega;",

	"&alpha;",
	"&beta;",
	"&gamma;",
	"&delta;",
	"&epsilon;",
	"&zeta;",
	"&eta;",
	"&theta;",
	"&iota;",
	"&kappa;",
	"&lambda;",
	"&mu;",
	"&nu;",
	"&xi;",
	"&omicron;",
	"&pi;",
	"&rho;",
	"&sigma;",
	"&tau;",
	"&upsilon;",
	"&phi;",
	"&chi;",
	"&psi;",
	"&omega;",

	"&hbar;",	// 176

	"&plus;",	// 177
	"&minus;",	// 178
	"&times;",	// 179
	"&ge;",		// 180
	"&le;",		// 181
];

function
draw_char(x, y, font_num, char_num)
{
	var s, t;

	x = Math.round(x);
	y = Math.round(y);

	if (char_num < 32 || char_num > 181)
		s = "?";
	else if (char_num == 34)
		s = "&quot;";
	else if (char_num == 38)
		s = "&amp;"
	else if (char_num == 60)
		s = "&lt;";
	else if (char_num == 62)
		s = "&gt;";
	else if (char_num < 128)
		s = String.fromCharCode(char_num);
	else
		s = html_name_tab[char_num - 128];

	t = "<text style='font-family:\"Times New Roman\";";

	switch (font_num) {
	case ROMAN_FONT:
		t += "font-size:" + FONT_SIZE + "px;";
		break;
	case ITALIC_FONT:
		t += "font-size:" + FONT_SIZE + "px;font-style:italic;";
		break;
	case SMALL_ROMAN_FONT:
		t += "font-size:" + SMALL_FONT_SIZE + "px;";
		break;
	case SMALL_ITALIC_FONT:
		t += "font-size:" + SMALL_FONT_SIZE + "px;font-style:italic;";
		break;
	}

	x = "x='" + x + "'";
	y = "y='" + y + "'";

	t += "'" + x + y + ">" + s + "</text>\n";

	outbuf += t;
}

function
draw_delims(x, y, h, d, w, stroke_width, font_num)
{
	var cd, ch, cw;

	ch = get_cap_height(font_num);
	cd = get_char_depth(font_num, LEFT_PAREN);
	cw = get_char_width(font_num, LEFT_PAREN);

	if (h > ch || d > cd) {
		draw_left_delim(x, y, h, d, cw, stroke_width);
		draw_right_delim(x + w - cw, y, h, d, cw, stroke_width);
	} else {
		draw_char(x, y, font_num, LEFT_PAREN);
		draw_char(x + w - cw, y, font_num, RIGHT_PAREN);
	}
}

function
draw_left_delim(x, y, h, d, w, stroke_width)
{
	var x1, x2, y1, y2;

	x1 = x + 0.25 * w;
	x2 = x + 0.75 * w;

	y1 = y - h + stroke_width / 2.0;
	y2 = y + d - stroke_width / 2.0;

	draw_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	draw_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	draw_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

function
draw_right_delim(x, y, h, d, w, stroke_width)
{
	var x1, x2, y1, y2;

	x1 = x + 0.75 * w;
	x2 = x + 0.25 * w;

	y1 = y - h + stroke_width / 2.0;
	y2 = y + d - stroke_width / 2.0;

	draw_stroke(x1, y1, x1, y2, stroke_width); // stem stroke
	draw_stroke(x1, y1, x2, y1, stroke_width); // top stroke
	draw_stroke(x1, y2, x2, y2, stroke_width); // bottom stroke
}

function
draw_stroke(x1, y1, x2, y2, stroke_width)
{
	var s;

	x1 = Math.round(x1);
	x2 = Math.round(x2);

	y1 = Math.round(y1);
	y2 = Math.round(y2);

	x1 = "x1='" + x1 + "'";
	x2 = "x2='" + x2 + "'";

	y1 = "y1='" + y1 + "'";
	y2 = "y2='" + y2 + "'";

	s = "<line " + x1 + y1 + x2 + y2 + "style='stroke:black;stroke-width:" + stroke_width + "'/>\n"

	outbuf += s;
}

function
draw_table(x, y, p)
{
	var cx, d, dx, h, i, j, m, n, w;
	var column_width, elem_width, row_depth, row_height, table;

	n = val1(p);
	m = val2(p);

	p = cddr(p);

	table = car(p);
	h = cadr(p);
	d = caddr(p);

	for (i = 0; i < n; i++) { // for each row

		row_height = val1(h);
		row_depth = val1(d);

		y += TABLE_VSPACE + row_height;

		dx = 0;

		w = cadddr(p);

		for (j = 0; j < m; j++) { // for each column

			column_width = val1(w);
			elem_width = width(car(table));
			cx = x + dx + TABLE_HSPACE + (column_width - elem_width) / 2; // center horizontal
			draw_formula(cx, y, car(table));
			dx += column_width + 2 * TABLE_HSPACE;
			table = cdr(table);
			w = cdr(w);
		}

		y += row_depth + TABLE_VSPACE;

		h = cdr(h);
		d = cdr(d);
	}
}
