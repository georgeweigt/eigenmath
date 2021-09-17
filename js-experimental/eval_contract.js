function
eval_contract(p1)
{
	var t = expanding;
	expanding = 1;
	eval_contract_nib(p1);
	expanding = t;
}

function
eval_contract_nib(p1)
{
	push(cadr(p1));
	evalf();

	p1 = cddr(p1);

	if (!iscons(p1)) {
		push_integer(1);
		push_integer(2);
		contract();
		return;
	}

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		push(cadr(p1));
		evalf();
		contract();
		p1 = cddr(p1);
	}
}
