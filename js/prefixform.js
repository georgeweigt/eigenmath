function
prefixform(p)
{
	if (iscons(p)) {
		var s = "(";
		s += prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			s += " ";
			s += prefixform(car(p));
			p = cdr(p);
		}
		s += ")";
		return s;
	}

	if (isinteger(p))
		return p.a.toFixed(0);

	if (isrational(p))
		return p.a.toFixed(0) + "/" + p.b.toFixed(0);

	if (isdouble(p))
		return p.d.toPrecision(6);

	if (issymbol(p))
		return "" + p.printname;

	if (isstring(p))
		return "'" + p.string + "'";

	if (istensor(p))
		return "(tensor)";

	return "?";
}
