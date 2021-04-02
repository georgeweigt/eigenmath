function
eval_run(p1)
{
	var f, k, save_inbuf, save_trace1, save_trace2, t;

	t = expanding;
	expanding = 1;

	push(cadr(p1));
	evalf();
	p1 = pop();

	if (!isstring(p1))
		stopf("run: string expected");

	f = new XMLHttpRequest();
	f.open("GET", p1.string, false);
	f.onerror = function() {stopf("run: network error")};
	f.send();

	if (f.status == 404 || f.responseText.length == 0)
		stopf("run: file not found");

	save_inbuf = inbuf;
	save_trace1 = trace1;
	save_trace2 = trace2;

	inbuf = f.responseText;

	k = 0;

	for (;;) {

		k = scan_inbuf(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}

	inbuf = save_inbuf;
	trace1 = save_trace1;
	trace2 = save_trace2;

	push_symbol(NIL);

	expanding = t;
}
