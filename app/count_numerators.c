#include "app.h"

int
count_numerators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isnumerator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}
