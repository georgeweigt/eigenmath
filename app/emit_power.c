#include "app.h"

void
emit_power(struct atom *p)
{
	if (cadr(p) == symbol(EXP1)) {
		emit_roman_string("exp");
		emit_args(cdr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_italic_char('j');
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_italic_char('i');
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		emit_reciprocal(p);
		return;
	}

	emit_base(cadr(p));
	emit_exponent(caddr(p));
}
