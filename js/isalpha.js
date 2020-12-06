function
isalpha(s)
{
	var c = s.charCodeAt(0);
	return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}
