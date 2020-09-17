const BLACK = 1;
const BLUE = 2;
const RED = 3;

function
print_buf(s, color)
{
	var t;

	s = s.replace(/&/g, "&amp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");
	s = s.replace(/\n/g, "<br>");

	switch (color) {

	case BLACK:
		t = "<p><span style='color:black'>";
		break;

	case BLUE:
		t = "<p><span style='color:blue;font-family:courier'>";
		break;

	case RED:
		t = "<p><span style='color:red;font-family:courier'>";
		break;
	}

	fputs(t + s + "</span>");
}
