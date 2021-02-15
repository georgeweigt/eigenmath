#include "app.h"

void
emit_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY))
		emit_term_nib(p);
	else
		emit_factor(p);
}

void
emit_term_nib(struct atom *p)
{
	if (find_denominator(p)) {
		emit_frac(p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)) && !isdouble(car(p)))
		p = cdr(p); // sign already emitted

	emit_factor(car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_medium_space();
		emit_factor(car(p));
		p = cdr(p);
	}
}
