const T_INTEGER = 1001;
const T_DOUBLE = 1002;
const T_SYMBOL = 1003;
const T_FUNCTION = 1004;
const T_NEWLINE = 1005;
const T_STRING = 1006;
const T_GTEQ = 1007;
const T_LTEQ = 1008;
const T_EQ = 1009;
const T_END = 1010;

var scan_mode;
var instring;
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
scan1(s)
{
	scan_mode = 1; // mode for table of integrals
	return scan_nib(s, 0);
}

function
scan_nib(s, k)
{
	instring = s;
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

function
scan_stmt()
{
	scan_comparison();
	if (token == "=") {
		get_token_skip_newlines(); // get token after =
		push_symbol(SETQ);
		swap();
		scan_comparison();
		list(3);
	}
}

function
scan_comparison()
{
	scan_expression();
	switch (token) {
	case T_EQ:
		push_symbol(TESTEQ); // ==
		break;
	case T_LTEQ:
		push_symbol(TESTLE);
		break;
	case T_GTEQ:
		push_symbol(TESTGE);
		break;
	case "<":
		push_symbol(TESTLT);
		break;
	case ">":
		push_symbol(TESTGT);
		break;
	default:
		return;
	}
	swap();
	get_token_skip_newlines(); // get token after rel op
	scan_expression();
	list(3);
}

function
scan_expression()
{
	var h = stack.length, t = token;
	if (token == "+" || token == "-")
		get_token_skip_newlines();
	scan_term();
	if (t == "-")
		static_negate();
	while (token == "+" || token == "-") {
		t = token;
		get_token_skip_newlines(); // get token after + or -
		scan_term();
		if (t == "-")
			static_negate();
	}
	if (stack.length - h > 1) {
		list(stack.length - h);
		push_symbol(ADD);
		swap();
		cons();
	}
}

function
scan_term()
{
	var h = stack.length, t;

	scan_power();

	while (scan_factor_pending()) {

		t = token;

		if (token == "*" || token == "/")
			get_token_skip_newlines();

		scan_power();

		if (t == "/")
			static_reciprocate();
	}

	if (stack.length - h > 1) {
		list(stack.length - h);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}
}

function
scan_factor_pending()
{
	switch (token) {
	case "*":
	case "/":
	case "(":
	case T_SYMBOL:
	case T_FUNCTION:
	case T_INTEGER:
	case T_DOUBLE:
	case T_STRING:
		return 1;
	default:
		break;
	}
	return 0;
}

function
scan_power()
{
	scan_factor();

	if (token == "^") {

		get_token_skip_newlines();

		push_symbol(POWER);
		swap();
		scan_power();
		list(3);
	}
}

function
scan_factor()
{
	var h, n;

	h = stack.length;

	switch (token) {

	case "(":
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		n = parseFloat(token_buf);
		if (n > MAXINT)
			push_double(n);
		else
			push_integer(n);
		get_token();
		break;

	case T_DOUBLE:
		n = parseFloat(token_buf);
		push_double(n);
		get_token();
		break;

	case T_STRING:
		scan_string();
		break;

	default:
		scan_error("expected operand");
		break;
	}

	// index

	if (token == "[") {

		scan_level++;

		get_token(); // get token after [
		push_symbol(INDEX);
		swap();

		scan_expression();

		while (token == ",") {
			get_token(); // get token after ,
			scan_expression();
		}

		if (token != "]")
			scan_error("expected ]");

		scan_level--;

		get_token(); // get token after ]

		list(stack.length - h);
	}

	while (token == "!") {
		get_token(); // get token after !
		push_symbol(FACTORIAL);
		swap();
		list(2);
	}
}

function
scan_symbol()
{
	if (scan_mode == 1 && token_buf.length == 1) {
		switch (token_buf[0]) {
		case "a":
			push_symbol(METAA);
			break;
		case "b":
			push_symbol(METAB);
			break;
		case "x":
			push_symbol(METAX);
			break;
		default:
			push(lookup(token_buf));
			break;
		}
	} else
		push(lookup(token_buf));
	get_token();
}

function
scan_string()
{
	push_string(token_buf);
	get_token();
}

function
scan_function_call()
{
	var h = stack.length;
	scan_level++;
	push(lookup(token_buf)); // push function name
	get_token(); // get token after function name
	get_token(); // get token after (
	if (token == ")") {
		scan_level--;
		get_token(); // get token after )
		list(1); // function call with no args
		return;
	}
	scan_stmt();
	while (token == ",") {
		get_token(); // get token after ,
		scan_stmt();
	}
	if (token != ")")
		scan_error("expected )");
	scan_level--;
	get_token(); // get token after )
	list(stack.length - h);
}

function
scan_subexpr()
{
	var h = stack.length;

	scan_level++;

	get_token(); // get token after (

	scan_stmt();

	while (token == ",") {
		get_token(); // get token after ,
		scan_stmt();
	}

	if (token != ")")
		scan_error("expected )");

	scan_level--;

	get_token(); // get token after )

	if (stack.length - h > 1)
		vector(h);
}

function
get_token_skip_newlines()
{
	scan_level++;
	get_token();
	scan_level--;
}

function
get_token()
{
	get_token_nib();

	if (scan_level)
		while (token == T_NEWLINE)
			get_token_nib(); // skip over newlines
}

function
get_token_nib()
{
	var c;

	// skip spaces

	while (inchar() == "\t" || inchar() == " ")
		scan_index++;

	c = inchar();

	token_index = scan_index;

	// end of input?

	if (c == "") {
		token = T_END;
		return;
	}

	scan_index++;

	// newline?

	if (c == "\n") {
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (c == "#" || (c == "-" && inchar() == "-")) {

		while (inchar() != "" && inchar() != "\n")
			scan_index++;

		if (inchar() != "")
			scan_index++;

		token = T_NEWLINE;

		return;
	}

	// number?

	if (isdigit(c) || c == ".") {

		while (isdigit(inchar()))
			scan_index++;

		if (inchar() == ".") {

			scan_index++;

			while (isdigit(inchar()))
				scan_index++;

			if (scan_index - token_index == 1)
				scan_error("expected decimal digit"); // only a decimal point

			token = T_DOUBLE;
		} else
			token = T_INTEGER;

		update_token_buf(token_index, scan_index);

		return;
	}

	// symbol?

	if (isalpha(c)) {

		while (isalnum(inchar()))
			scan_index++;

		if (inchar() == "(")
			token = T_FUNCTION;
		else
			token = T_SYMBOL;

		update_token_buf(token_index, scan_index);

		return;
	}

	// string ?

	if (c == "\"") {
		while (inchar() != "" && inchar() != "\n" && inchar() != "\"")
			scan_index++;
		if (inchar() != "\"") {
			token_index = scan_index; // no token
			scan_error("runaway string");
		}
		scan_index++;
		token = T_STRING;
		update_token_buf(token_index + 1, scan_index - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (c == "=" && inchar() == "=") {
		scan_index++;
		token = T_EQ;
		return;
	}

	if (c == "<" && inchar() == "=") {
		scan_index++;
		token = T_LTEQ;
		return;
	}

	if (c == ">" && inchar() == "=") {
		scan_index++;
		token = T_GTEQ;
		return;
	}

	// single char token

	token = c;
}

function
update_token_buf(j, k)
{
	token_buf = instring.substring(j, k);
}

function
scan_error(s)
{
	var t = inbuf.substring(trace1, scan_index);

	t += "\nStop: Syntax error, " + s;

	if (token_index < scan_index) {
		t += " instead of ";
		t += instring.substring(token_index, scan_index);
	}

	print_buf(t, RED);

	stopf("");
}

function
inchar()
{
	return instring.charAt(scan_index); // returns empty string if index out of range
}
