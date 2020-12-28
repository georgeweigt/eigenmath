function
run_nib()
{
	var k, t;

	stdout = document.getElementById("stdout");

	stdout.innerHTML = "";

	init();
	initscript();

	t = document.getElementById("stdlib");

	if (t != null) {
		inbuf = t.contentDocument.body.childNodes[0].textContent;
		k = 0;
		for (;;) {
			k = scan_inbuf(k);
			if (k == 0)
				break; // end of input
			eval_and_print_result();
		}
	}

	inbuf = document.getElementById("stdin").value;

	k = 0;

	for (;;) {

		k = scan_inbuf(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
