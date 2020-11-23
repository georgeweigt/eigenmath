#include "app.h"

int
isdenominator(struct atom *p)
{
	return car(p) == symbol(POWER) && isnegativenumber(caddr(p));
}
