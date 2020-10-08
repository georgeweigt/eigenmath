function
run_nib()
{
	var k = 0;

	init();
	initscript();

	for (;;) {

		k = scan_input(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
