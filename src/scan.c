// token_str and scan_str are pointers to the input string, for example
//
//	| g | a | m | m | a |   | a | l | p | h | a |
//	  ^                   ^
//	  token_str           scan_str

#define T_INTEGER 1001
#define T_DOUBLE 1002
#define T_SYMBOL 1003
#define T_FUNCTION 1004
#define T_NEWLINE 1006
#define T_STRING 1007
#define T_GTEQ 1008
#define T_LTEQ 1009
#define T_EQ 1010
#define T_END 1011

int token;
int scan_mode;
int scan_level;

char *scan_str;
char *token_str;
char *token_buf;
int token_buf_len;

char *
scan(char *s)
{
	scan_mode = 0;
	return scan_nib(s);
}

char *
scan1(char *s)
{
	scan_mode = 1; // mode for table of integrals
	return scan_nib(s);
}

char *
scan_nib(char *s)
{
	scan_str = s;
	scan_level = 0;
	get_token_skip_newlines();
	if (token == T_END)
		return NULL;
	scan_stmt();
	if (token != T_NEWLINE && token != T_END)
		scan_error("syntax err"); // unexpected token, for example, 1:2
	return scan_str;
}

void
scan_stmt(void)
{
	scan_comparison();
	if (token == '=') {
		get_token_skip_newlines(); // get token after '='
		push_symbol(SETQ);
		swap();
		scan_comparison();
		list(3);
	}
}

void
scan_comparison(void)
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
	case '<':
		push_symbol(TESTLT);
		break;
	case '>':
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

void
scan_expression(void)
{
	int h = tos, t;
	t = token;
	if (token == '+' || token == '-')
		get_token_skip_newlines();
	scan_term();
	if (t == '-')
		static_negate();
	while (token == '+' || token == '-') {
		t = token;
		get_token_skip_newlines(); // get token after '+' or '-'
		scan_term();
		if (t == '-')
			static_negate();
	}
	if (tos - h > 1) {
		list(tos - h);
		push_symbol(ADD);
		swap();
		cons(); // prepend ADD to list
	}
}

int
another_factor_pending(void)
{
	switch (token) {
	case '*':
	case '/':
	case '(':
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

void
scan_term(void)
{
	int h = tos, t;

	scan_power();

	while (another_factor_pending()) {

		t = token;

		if (token == '*' || token == '/')
			get_token_skip_newlines();

		scan_power();

		if (t == '/')
			static_reciprocate();
	}

	if (tos - h > 1) {
		list(tos - h);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
	}
}

void
scan_power(void)
{
	scan_factor();
	if (token == '^') {
		get_token_skip_newlines();
		push_symbol(POWER);
		swap();
		scan_power();
		list(3);
	}
}

void
scan_factor(void)
{
	int h = tos;

	switch (token) {

	case '(':
		scan_subexpr();
		break;

	case T_SYMBOL:
		scan_symbol();
		break;

	case T_FUNCTION:
		scan_function_call();
		break;

	case T_INTEGER:
		scan_integer();
		get_token();
		break;

	case T_DOUBLE:
		push_double(atof(token_buf));
		get_token();
		break;

	case T_STRING:
		scan_string();
		break;

	default:
		scan_error("syntax err");
		break;
	}

	// index

	if (token == '[') {
		scan_level++;
		get_token(); // get token after '['
		push_symbol(INDEX);
		swap();
		scan_expression();
		while (token == ',') {
			get_token(); // get token after ','
			scan_expression();
		}
		if (token != ']')
			scan_error("expected ']'");
		scan_level--;
		get_token(); // get token after ']'
		list(tos - h);
	}

	while (token == '!') {
		get_token(); // get token after '!'
		push_symbol(FACTORIAL);
		swap();
		list(2);
	}
}

void
scan_symbol(void)
{
	if (scan_mode && strlen(token_buf) == 1)
		switch (token_buf[0]) {
		case 'a':
			push_symbol(SA);
			break;
		case 'b':
			push_symbol(SB);
			break;
		case 'x':
			push_symbol(SX);
			break;
		default:
			push(lookup(token_buf));
			break;
		}
	else
		push(lookup(token_buf));
	get_token();
}

void
scan_string(void)
{
	push_string(token_buf);
	get_token();
}

void
scan_function_call(void)
{
	int h = tos;
	scan_level++;
	push(lookup(token_buf)); // push function name
	get_token(); // get token after function name
	get_token(); // get token after '('
	if (token == ')') {
		scan_level--;
		get_token(); // get token after ')'
		list(1); // function call with no args
		return;
	}
	scan_stmt();
	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != ')')
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	list(tos - h);
}

void
scan_integer(void)
{
	int sign;
	uint32_t *a;
	switch (*token_buf) {
	case '+':
		sign = MPLUS;
		a = mscan(token_buf + 1);
		break;
	case '-':
		sign = MMINUS;
		a = mscan(token_buf + 1);
		break;
	default:
		sign = MPLUS;
		a = mscan(token_buf);
		break;
	}
	push_bignum(sign, a, mint(1));
}

void
scan_subexpr(void)
{
	int h = tos;
	scan_level++;
	get_token(); // get token after '('
	scan_stmt();
	while (token == ',') {
		get_token(); // get token after ','
		scan_stmt();
	}
	if (token != ')')
		scan_error("expected ')'");
	scan_level--;
	get_token(); // get token after ')'
	if (tos - h > 1)
		build_tensor(h);
}

void
get_token_skip_newlines(void)
{
	scan_level++;
	get_token();
	scan_level--;
}

void
get_token(void)
{
	get_token_nib();
	if (scan_level)
		while (token == T_NEWLINE)
			get_token_nib();
}

void
get_token_nib(void)
{
	// skip spaces

	while (*scan_str != '\0' && *scan_str != '\n' && *scan_str != '\r' && (*scan_str < 33 || *scan_str > 126))
		scan_str++;

	token_str = scan_str;

	// end of input?

	if (*scan_str == '\0') {
		token = T_END;
		return;
	}

	// newline?

	if (*scan_str == '\n' || *scan_str == '\r') {
		scan_str++;
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (*scan_str == '#' || (scan_str[0] == '-' && scan_str[1] == '-')) {
		while (*scan_str && *scan_str != '\n' && *scan_str != '\r')
			scan_str++;
		if (*scan_str)
			scan_str++;
		token = T_NEWLINE;
		return;
	}

	// number?

	if (isdigit(*scan_str) || *scan_str == '.') {
		while (isdigit(*scan_str))
			scan_str++;
		if (*scan_str == '.') {
			scan_str++;
			while (isdigit(*scan_str))
				scan_str++;
			if (token_str + 1 == scan_str)
				scan_error("expected decimal digit"); // only a decimal point
			token = T_DOUBLE;
		} else
			token = T_INTEGER;
		update_token_buf(token_str, scan_str);
		return;
	}

	// symbol?

	if (isalpha(*scan_str)) {
		while (isalnum(*scan_str))
			scan_str++;
		if (*scan_str == '(')
			token = T_FUNCTION;
		else
			token = T_SYMBOL;
		update_token_buf(token_str, scan_str);
		return;
	}

	// string?

	if (*scan_str == '"') {
		scan_str++;
		while (*scan_str && *scan_str != '"' && *scan_str != '\n' && *scan_str != '\r')
			scan_str++;
		if (*scan_str != '"')
			scan_error("runaway string");
		scan_str++;
		token = T_STRING;
		update_token_buf(token_str + 1, scan_str - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (*scan_str == '=' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_EQ;
		return;
	}

	if (*scan_str == '<' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_LTEQ;
		return;
	}

	if (*scan_str == '>' && scan_str[1] == '=') {
		scan_str += 2;
		token = T_GTEQ;
		return;
	}

	// single char token

	token = *scan_str++;
}

void
update_token_buf(char *a, char *b)
{
	int m, n;

	n = (int) (b - a);

	// Let n == 1000

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * (n / 1000 + 1); // m is a multiple of 1000

	if (m > token_buf_len) {
		if (token_buf)
			free(token_buf);
		token_buf = alloc_mem(m);
		token_buf_len = m;
	}

	strncpy(token_buf, a, n);
	token_buf[n] = '\0';
}

void
scan_error(char *errmsg)
{
	trace2 = scan_str;
	stopf(errmsg);
}

// There are n expressions on the stack, possibly tensors.
// This function assembles the stack expressions into a single tensor.
// For example, at the top level of the expression ((a,b),(c,d)), the vectors
// (a,b) and (c,d) would be on the stack.

void
build_tensor(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h, *p2;

	p2 = alloc_tensor(n);
	p2->u.tensor->ndim = 1;
	p2->u.tensor->dim[0] = n;
	for (i = 0; i < n; i++)
		p2->u.tensor->elem[i] = s[i];

	tos = h;

	push(p2);
}

void
static_negate(void)
{
	struct atom *p1;

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

void
static_reciprocate(void)
{
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	// save divide by zero error for runtime

	if (iszero(p2)) {
		if (!isinteger1(p1))
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

	if (isnum(p2)) {
		if (!isinteger1(p1))
			push(p1);
		push(p2);
		reciprocate();
		return;
	}

	if (car(p2) == symbol(POWER) && isnum(caddr(p2))) {
		if (!isinteger1(p1))
			push(p1);
		push_symbol(POWER);
		push(cadr(p2));
		push(caddr(p2));
		negate();
		list(3);
		return;
	}

	if (!isinteger1(p1))
		push(p1);

	push_symbol(POWER);
	push(p2);
	push_integer(-1);
	list(3);
}
