#include "app.h"

void
emit_symbol(struct atom *p)
{
	int k, t;
	char *s;

	if (p == symbol(EXP1)) {
		emit_roman_string("exp(1)");
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman_string(s);
		return;
	}

	k = emit_symbol_fragment(s, 0);

	if (s[k] == '\0')
		return;

	// emit subscript

	t = tos;

	emit_level++;

	while (s[k] != '\0')
		k = emit_symbol_fragment(s, k);

	emit_level--;

	emit_update_subscript(t);
}
