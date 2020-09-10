var init_script = [
"e=exp(1)",
"i=sqrt(-1)",
"trange=(-pi,pi)",
"xrange=(-10,10)",
"yrange=(-10,10)",
"cross(u,v)=(u[2]*v[3]-u[3]*v[2],u[3]*v[1]-u[1]*v[3],u[1]*v[2]-u[2]*v[1])",
"curl(u)=(d(u[3],y)-d(u[2],z),d(u[1],z)-d(u[3],x),d(u[2],x)-d(u[1],y))",
"div(u)=d(u[1],x)+d(u[2],y)+d(u[3],z)",
"ln(x)=log(x)",
"last=0",
"tty=0",
];

function
run_init_script()
{
	var i, n;
	n = init_script.length;
	for (i = 0; i < n; i++) {
		scan(init_script[i]);
		evalf();
		pop();
	}
}
