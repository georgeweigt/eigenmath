function
power_complex_double(BASE, EXPO, X, Y)
{
	var expo, r, theta, x, y;

	push(X);
	x = pop_double();

	push(Y);
	y = pop_double();

	push(EXPO);
	expo = pop_double();

	r = Math.sqrt(x * x + y * y);
	theta = Math.atan2(y, x);

	r = Math.pow(r, expo);
	theta = expo * theta;

	x = r * Math.cos(theta);
	y = r * Math.sin(theta);

	push_double(x);
	push_double(y);
	push(imaginaryunit);
	multiply();
	add();
}
