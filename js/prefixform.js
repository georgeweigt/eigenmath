function
prefixform(p)
{
	if (iscons(p)) {
		outbuf += "(";
		prefixform(car(p));
		p = cdr(p);
		while (iscons(p)) {
			outbuf += " ";
			prefixform(car(p));
			p = cdr(p);
		}
		outbuf += ")";
	} else if (isinteger(p))
		outbuf += p.a.toFixed(0);
	else if (isrational(p))
		outbuf += p.a.toFixed(0) + "/" + p.b.toFixed(0);
	else if (isdouble(p))
		outbuf += p.d.toPrecision(6);
	else if (issymbol(p))
		outbuf += p.printname;
	else if (isstring(p))
		outbuf += "'" + p.string + "'";
	else if (istensor(p))
		outbuf += "[ ]";
	else
		outbuf += " ? ";
}
