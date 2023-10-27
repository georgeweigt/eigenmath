var init_script = [
"i = sqrt(-1)",
"grad(f) = d(f,(x,y,z))",
"cross(a,b) = (dot(a[2],b[3])-dot(a[3],b[2]),dot(a[3],b[1])-dot(a[1],b[3]),dot(a[1],b[2])-dot(a[2],b[1]))",
"curl(u) = (d(u[3],y) - d(u[2],z),d(u[1],z) - d(u[3],x),d(u[2],x) - d(u[1],y))",
"div(u) = d(u[1],x) + d(u[2],y) + d(u[3],z)",
"laguerre(x,n,m) = (n + m)! sum(k,0,n,(-x)^k / ((n - k)! (m + k)! k!))",
"legendre(f,n,m,x) = eval(1 / (2^n n!) (1 - x^2)^(m/2) d((x^2 - 1)^n,x,n + m),x,f)",
"hermite(x,n) = (-1)^n exp(x^2) d(exp(-x^2),x,n)",
"binomial(n,k) = n! / k! / (n - k)!",
"choose(n,k) = n! / k! / (n - k)!",
];

function
initscript()
{
	var i, n;

	n = init_script.length;

	for (i = 0; i < n; i++) {
		scan(init_script[i], 0);
		evalf();
		pop();
	}
}
