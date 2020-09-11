var init_script = [
"i = sqrt(-1)",
"trace = 0",
];

function
initscript()
{
	var i, n

	n = init_script.length;

	for (i = 0; i < n; i++) {
		scan(init_script[i], 0);
		evalf();
		pop();
	}
}
