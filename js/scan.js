const T_INTEGER = 1001;
const T_DOUBLE = 1002;
const T_SYMBOL = 1003;
const T_FUNCTION = 1004;
const T_NEWLINE = 1005
const T_STRING = 1006
const T_GTEQ = 1007
const T_LTEQ = 1008
const T_EQ = 1009
const T_END = 1010

var scan_mode;
var instring;
var scan_length;
var scan_index;
var scan_level;
var token;
var token_index;
var token_buf;

function
scan(s, k)
{
	scan_mode = 0;
	return scan_nib(s, k);
}


function
scan1(s, k)
{
	scan_mode = 1; // mode for table of integrals
	return scan_nib(s, k);
}

function
scan_nib(s, k)
{
	instring = s;

	scan_length = s.length;
	scan_index = k;
	scan_level = 0;

	get_token_skip_newlines();

	if (token == T_END)
		return 0;

	scan_stmt();

	if (token != T_NEWLINE && token != T_END)
		scan_error("expected newline");

	return scan_index;
}
