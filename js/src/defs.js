function
car(p)
{
	if ("car" in p)
		return p.car;
	else
		return symbol(NIL);
}

function
cdr(p)
{
	if ("cdr" in p)
		return p.cdr;
	else
		return symbol(NIL);
}

function
cadr(p)
{
	return car(cdr(p));
}

function
cddr(p)
{
	return cdr(cdr(p));
}

function
caadr(p)
{
	return car(car(cdr(p)));
}

function
caddr(p)
{
	return car(cdr(cdr(p)));
}

function
cdadr(p)
{
	return cdr(car(cdr(p)));
}

function
cdddr(p)
{
	return cdr(cdr(cdr(p)));
}

function
caaddr(p)
{
	return car(car(cdr(cdr(p))));
}

function
cadadr(p)
{
	return car(cdr(car(cdr(p))));
}

function
cadddr(p)
{
	return car(cdr(cdr(cdr(p))));
}

function
cddadr(p)
{
	return cdr(cdr(car(cdr(p))));
}

function
cddddr(p)
{
	return cdr(cdr(cdr(cdr(p))));
}

function
cadaddr(p)
{
	return car(cdr(car(cdr(cdr(p)))));
}

function
caddddr(p)
{
	return car(cdr(cdr(cdr(cdr(p)))));
}
