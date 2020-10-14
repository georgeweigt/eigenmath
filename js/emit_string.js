function
emit_string(p)
{
	var s = p.string;

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	emit_roman(s);
}
