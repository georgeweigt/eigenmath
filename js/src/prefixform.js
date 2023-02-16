function
prefixform(p)
{
	var s;
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
	} else if (isdouble(p)) {
		s = p.d.toPrecision(6);
		if (s.indexOf("E") < 0 && s.indexOf("e") < 0 && s.indexOf(".") >= 0) {
			// remove trailing zeroes
			while (s.charAt(s.length - 1) == "0")
				s = s.substring(0, s.length - 1);
			if (s.charAt(s.length - 1) == '.')
				s += "0";
		}
		outbuf += s;
	} else if (issymbol(p))
		outbuf += p.printname;
	else if (isstring(p))
		outbuf += "'" + p.string + "'";
	else if (istensor(p))
		outbuf += "[ ]";
	else
		outbuf += " ? ";
}
