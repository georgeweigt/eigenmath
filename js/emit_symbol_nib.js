function
emit_symbol_nib(s)
{
	if (s.length == 1)
		emit_italic(s);
	else if (s[0] >= 'A' && s[0] <= 'Z')
		emit_roman("&" + s + ";");
	else
		emit_italic("&" + s + ";");
}
