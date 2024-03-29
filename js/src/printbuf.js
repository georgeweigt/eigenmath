const BLACK = 1;
const BLUE = 2;
const RED = 3;

function
printbuf(s, color)
{
	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");
	s = s.replace(/\r/g, "");

	if (!s.endsWith("<br>"))
		s += "<br>";

	switch (color) {

	case BLACK:
		s = "<span style='color:black;font-family:courier'>" + s + "</span>";
		break;

	case BLUE:
		s = "<span style='color:blue;font-family:courier'>" + s + "</span>";
		break;

	case RED:
		s = "<span style='color:red;font-family:courier'>" + s + "</span>";
		break;
	}

	stdout.innerHTML += s;
}
