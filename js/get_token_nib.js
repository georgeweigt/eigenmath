function
get_token_nib()
{
	var c;

	// skip spaces

	while (scan_index < scan_length && (instring[scan_index] == '\t' || instring[scan_index] == ' '))
		scan_index++;

	token_index = scan_index;

	// end of string?

	if (scan_index == scan_length) {
		token = T_END;
		return;
	}

	c = instring[scan_index];

	// newline?

	if (c == '\n') {
		scan_index++;
		token = T_NEWLINE;
		return;
	}

	// comment?

	if (c == '#' || (c == '-' && scan_index < scan_length - 1 && instring[scan_index + 1] == '-')) {

		while (scan_index < scan_length && instring[scan_index] != '\n')
			scan_index++;

		if (scan_index < scan_length)
			scan_index++;

		token = T_NEWLINE;

		return;
	}

	// number?

	if (isdigit(c) || c == '.') {

		while (scan_index < scan_length && isdigit(instring[scan_index]))
			scan_index++;

		if (scan_index < scan_length && instring[scan_index] == '.') {

			scan_index++;

			while (scan_index < scan_length && isdigit(instring[scan_index]))
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

		while (scan_index < scan_length && isalnum(instring[scan_index]))
			scan_index++;

		if (scan_index < scan_length && instring[scan_index] == '(')
			token = T_FUNCTION;
		else
			token = T_SYMBOL;

		update_token_buf(token_index, scan_index);

		return;
	}

	// string ?

	if (c == '"') {
		scan_index++;
		while (scan_index < scan_length && instring[scan_index] != '"') {
			if (scan_index == scan_length || instring[scan_index] == '\n') {
				token_index = scan_index;
				scan_error("runaway string");
			}
			scan_index++;
		}
		scan_index++;
		token = T_STRING;
		update_token_buf(token_index + 1, scan_index - 1); // don't include quote chars
		return;
	}

	// relational operator?

	if (scan_index < scan_length) {

		if (c == '=' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_EQ;
			return;
		}

		if (c == '<' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_LTEQ;
			return;
		}

		if (c == '>' && instring[scan_index + 1] == '=') {
			scan_index += 2;
			token = T_GTEQ;
			return;
		}
	}

	// single char token

	token = instring[scan_index++];
}
