function
run_nib()
{
	var k = 0;

	inbuf = document.getElementById("stdin").value;
	stdout = document.getElementById("stdout");
	stdout.innerHTML = "";

	init();
	initscript();

	for (;;) {

		k = scan_inbuf(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
