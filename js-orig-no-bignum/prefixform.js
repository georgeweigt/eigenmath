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
	} else if (isrational(p)) {
		if (isnegativenumber(p))
			outbuf += '-';
		outbuf += bignum_itoa(p.a);
		if (isfraction(p))
			outbuf += "/" + bignum_itoa(p.b);
	} else if (isdouble(p))
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
