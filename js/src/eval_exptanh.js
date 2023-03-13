function
eval_exptanh(p1)
{
	push(cadr(p1));
	evalf();
	exptanh();
}

function
exptanh()
{
	var p1;
	push_integer(2);
	multiply();
	expfunc();
	p1 = pop();
	push(p1);
	push_integer(1);
	subtract();
	push(p1);
	push_integer(1);
	add();
	divide();
}
