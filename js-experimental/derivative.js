function
derivative()
{
	var F, X;

	X = pop();
	F = pop();

	if (istensor(F)) {
		if (istensor(X))
			dtt(F, X);
		else
			dts(F, X);
	} else {
		if (istensor(X))
			dst(F, X);
		else
			dss(F, X);
	}
}
