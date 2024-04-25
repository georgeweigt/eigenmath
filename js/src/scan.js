const T_EXCLAM = 33;
const T_QUOTEDBL = 34;
const T_NUMBERSIGN = 35;
const T_PARENLEFT = 40;
const T_PARENRIGHT = 41;
const T_ASTERISK = 42;
const T_PLUS = 43;
const T_COMMA = 44;
const T_HYPHEN = 45;
const T_PERIOD = 46;
const T_SLASH = 47;
const T_LESS = 60;
const T_EQUAL = 61;
const T_GREATER = 62;
const T_BRACKETLEFT = 91;
const T_BRACKETRIGHT = 93;
const T_ASCIICIRCUM = 94;
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
	if (token == T_EQUAL) {
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
	case T_LESS:
		push_symbol(TESTLT);
		break;
	case T_GREATER:
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
	if (token == T_PLUS || token == T_HYPHEN)
		get_token_skip_newlines();
	scan_term();
	if (t == T_HYPHEN)
		static_negate();
	while (token == T_PLUS || token == T_HYPHEN) {
		t = token;
		get_token_skip_newlines(); // get token after + or -
		scan_term();
		if (t == T_HYPHEN)
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

		if (token == T_ASTERISK || token == T_SLASH)
			get_token_skip_newlines();

		scan_power();

		if (t == T_SLASH)
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
	case T_ASTERISK:
	case T_SLASH:
	case T_PARENLEFT:
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

	if (token == T_ASCIICIRCUM) {

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
	var a, b, d, h;

	h = stack.length;

	switch (token) {

	case T_PARENLEFT:
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		a = bignum_atoi(token_buf);
		b = bignum_int(1);
		push_bignum(1, a, b);
		get_token();
		break;

	case T_DOUBLE:
		d = parseFloat(token_buf);
		push_double(d);
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

	if (token == T_BRACKETLEFT) {

		scan_level++;

		get_token(); // get token after [
		push_symbol(INDEX);
		swap();

		scan_expression();

		while (token == T_COMMA) {
			get_token(); // get token after ,
			scan_expression();
		}

		if (token != T_BRACKETRIGHT)
			scan_error("expected ]");

		scan_level--;

		get_token(); // get token after ]

		list(stack.length - h);
	}

	while (token == T_EXCLAM) {
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
			push_symbol(SA);
			break;
		case "b":
			push_symbol(SB);
			break;
		case "x":
			push_symbol(SX);
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
	if (token == T_PARENRIGHT) {
		scan_level--;
		get_token(); // get token after )
		list(1); // function call with no args
		return;
	}
	scan_stmt();
	while (token == T_COMMA) {
		get_token(); // get token after ,
		scan_stmt();
	}
	if (token != T_PARENRIGHT)
		scan_error("expected )");
	scan_level--;
	get_token(); // get token after )
	list(stack.length - h);
}

function
scan_subexpr()
{
	var h, i, n, p;
	h = stack.length;
	scan_level++;
	get_token(); // get token after '('
	scan_stmt();
	while (token == T_COMMA) {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != T_PARENRIGHT)
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	n = stack.length - h;
	if (n < 2)
		return;
	p = alloc_vector(n);
	for (i = 0; i < n; i++)
		p.elem[i] = stack[h + i];
	stack.length = h;
	push(p);
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
	// skip spaces

	while (scan_index < instring.length && instring.charCodeAt(scan_index) != 10 && instring.charCodeAt(scan_index) != 13 && (instring.charCodeAt(scan_index) < 33 || instring.charCodeAt(scan_index) > 126))
		scan_index++;

	token_index = scan_index;

	// end of input?

	if (scan_index == instring.length) {
		token = T_END;
		return;
	}

	// newline?

	if (instring.charCodeAt(scan_index) == 10 || instring.charCodeAt(scan_index) == 13) {
		scan_index++;
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (instring.charCodeAt(scan_index) == T_NUMBERSIGN || (instring.charCodeAt(scan_index) == T_HYPHEN && scan_index + 1 < instring.length && instring.charCodeAt(scan_index + 1) == T_HYPHEN)) {
		while (scan_index < instring.length && instring.charCodeAt(scan_index) != 10 && instring.charCodeAt(scan_index) != 13)
			scan_index++;
		if (scan_index < instring.length)
			scan_index++;
		token = T_NEWLINE;
		return;
	}

	// number?

	if (isdigit(instring.charCodeAt(scan_index)) || instring.charCodeAt(scan_index) == T_PERIOD) {
		while (scan_index < instring.length && isdigit(instring.charCodeAt(scan_index)))
			scan_index++;
		if (scan_index < instring.length && instring.charCodeAt(scan_index) == T_PERIOD) {
			scan_index++;
			while (scan_index < instring.length && isdigit(instring.charCodeAt(scan_index)))
				scan_index++;
			if (token_index + 1 == scan_index)
				scan_error("expected decimal digit"); // only a decimal point
			token = T_DOUBLE;
		} else
			token = T_INTEGER;
		update_token_buf(token_index, scan_index);
		return;
	}

	// symbol?

	if (isalpha(instring.charCodeAt(scan_index))) {
		while (scan_index < instring.length && isalnum(instring.charCodeAt(scan_index)))
			scan_index++;
		if (scan_index < instring.length && instring.charCodeAt(scan_index) == T_PARENLEFT)
			token = T_FUNCTION;
		else
			token = T_SYMBOL;
		update_token_buf(token_index, scan_index);
		return;
	}

	// string?

	if (instring.charCodeAt(scan_index) == T_QUOTEDBL) {
		scan_index++;
		while (scan_index < instring.length && instring.charCodeAt(scan_index) != T_QUOTEDBL && instring.charCodeAt(scan_index) != 10 && instring.charCodeAt(scan_index) != 13)
			scan_index++;
		if (scan_index == instring.length || instring.charCodeAt(scan_index) != T_QUOTEDBL)
			scan_error("runaway string");
		scan_index++;
		token = T_STRING;
		update_token_buf(token_index + 1, scan_index - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (instring.charCodeAt(scan_index) == T_EQUAL && scan_index + 1 < instring.length && instring.charCodeAt(scan_index + 1) == T_EQUAL) {
		scan_index += 2;
		token = T_EQ;
		return;
	}

	if (instring.charCodeAt(scan_index) == T_LESS && scan_index + 1 < instring.length && instring.charCodeAt(scan_index + 1) == T_EQUAL) {
		scan_index += 2;
		token = T_LTEQ;
		return;
	}

	if (instring.charCodeAt(scan_index) == T_GREATER && scan_index + 1 < instring.length && instring.charCodeAt(scan_index + 1) == T_EQUAL) {
		scan_index += 2;
		token = T_GTEQ;
		return;
	}

	// single char token

	token = instring.charCodeAt(scan_index++);
}

function
update_token_buf(j, k)
{
	token_buf = instring.substring(j, k);
}

function
scan_error(errmsg)
{
	trace2 = scan_index;
	stopf(errmsg);
}

function
static_negate()
{
	var p1;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		negate();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		push_symbol(MULTIPLY);
		if (isnum(cadr(p1))) {
			push(cadr(p1)); // A
			negate();
			push(cddr(p1)); // B
		} else {
			push_integer(-1); // A
			push(cdr(p1)); // B
		}
		cons(); // prepend A to B
		cons(); // prepend MULTIPLY
		return;
	}

	push_symbol(MULTIPLY);
	push_integer(-1);
	push(p1);
	list(3);
}

function
static_reciprocate()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		push(p1);
		push_symbol(POWER);
		push(p2);
		push_integer(-1);
		list(3);
		return;
	}

	if (isnum(p1) && isnum(p2)) {
		push(p1);
		push(p2);
		divide();
		return;
	}

	if (!isrational(p1) || !isequaln(p1, 1))
		push(p1); // p1 != 1

	if (isnum(p2)) {
		push(p2);
		reciprocate();
		return;
	}

	if (car(p2) == symbol(POWER) && isnum(caddr(p2))) {
		push_symbol(POWER);
		push(cadr(p2));
		push(caddr(p2));
		negate();
		list(3);
		return;
	}

	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
