#include "app.h"

void
emit_double(struct atom *p)
{
	int i, j, k, t;

	if (p->u.d == 0.0) {
		emit_roman_char('0');
		return;
	}

	sprintf(tbuf, "%g", fabs(p->u.d));

	k = 0;

	while (isdigit(tbuf[k]) || tbuf[k] == '.')
		k++;

	// handle trailing zeroes

	j = k;

	if (strchr(tbuf, '.'))
		while (tbuf[j - 1] == '0' && tbuf[j - 2] != '.')
			j--;

	for (i = 0; i < j; i++)
		emit_roman_char(tbuf[i]);

	if (tbuf[k] != 'E' && tbuf[k] != 'e')
		return;

	k++;

	emit_roman_char(TIMES);

	emit_roman_string("10");

	t = tos;

	emit_level++;

	// sign of exponent

	if (tbuf[k] == '+')
		k++;
	else if (tbuf[k] == '-') {
		emit_roman_char(MINUS);
		emit_thin_space();
		k++;
	}

	// skip leading zeroes in exponent

	while (tbuf[k] == '0')
		k++;

	emit_roman_string(tbuf + k);

	emit_level--;

	emit_update_superscript(t);
}
