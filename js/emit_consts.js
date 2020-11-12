const SPACE = 0;
const TEXT = 1;
const LINE = 2;
const SUBEXPR = 3;
const SUPERSCRIPT = 4;
const SUBSCRIPT = 5;
const FRACTION = 6;
const TABLE = 7;

const HPAD = 10;
const VPAD = 6;

const FONT_SIZE = 20;
const FONT_HEIGHT = 18;
const FONT_DEPTH = 6;
const MINUS_HEIGHT = 8.5;
const FRAC_VSPACE = 5.5;
const FRAC_STROKE = 1.5;
const DELIM_STROKE = 2;

const SMALL_FONT_SIZE = 14;
const SMALL_FONT_HEIGHT = 13;
const SMALL_FONT_DEPTH = 4;
const SMALL_MINUS_HEIGHT = 6;
const SMALL_FRAC_VSPACE = 4;
const SMALL_FRAC_STROKE = 1;
const SMALL_DELIM_STROKE = 1.5;

const TABLE_HSPACE = 12;
const TABLE_VSPACE = 12;
const TABLE_DELIM_STROKE = 2.5;

const WIDTH_RATIO = 0.0014;

const roman_width = [
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
250,333,408,503,500,833,778,180,
333,333,500,565,250,333,250,301,
500,500,500,500,500,500,500,500,
500,500,278,278,565,566,564,444,
921,732,667,667,722,611,556,729,
723,333,404,751,611,889,729,722,
556,722,697,556,611,732,731,957,
731,729,611,333,301,333,472,530,
333,463,500,444,523,444,425/*456*/,503,
515,278,278,527,278,794,517,500,
500,521,360,389,300,520,512,734,
509,515,449,480,200,480,563,778,
];

const italic_width = [
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
778,778,778,778,778,778,778,778,
250,338,473,503,510,838,778,282,
463,333,500,675,250,333,250,402,
515,500,500,500,508,500,546,544,
510,512,333,333,675,675,675,500,
920,611,611,725,726,653,689,746,
832,431,568,750,556,945,783,739,
642,738,611,563,670,835,744,965,
762,688,632,492,278,389,481,530,
358,500,500,449,562,451,494,542,
500,293,301,511,318,722,500,500,
500,500,424,389,321,500,463,683,
460,461,413,536,275,400,563,778,
];

const glyph_names = [
"Alpha",
"Beta",
"Gamma",
"Delta",
"Epsilon",
"Zeta",
"Eta",
"Theta",
"Iota",
"Kappa",
"Lambda",
"Mu",
"Nu",
"Xi",
"Pi",
"Rho",
"Sigma",
"Tau",
"Upsilon",
"Phi",
"Chi",
"Psi",
"Omega",
"alpha",
"beta",
"gamma",
"delta",
"epsilon",
"zeta",
"eta",
"theta",
"iota",
"kappa",
"lambda",
"mu",
"nu",
"xi",
"pi",
"rho",
"sigma",
"tau",
"upsilon",
"phi",
"chi",
"psi",
"omega",
"hbar",
];

const glyph_info = {
"&parenleft;":	{width:333,	italic_font:0,	descender:1},
"&parenright;":	{width:333,	italic_font:0,	descender:1},
"&plus;":	{width:565,	italic_font:0,	descender:0},
"&lt;":		{width:565,	italic_font:0,	descender:0},
"&equals;":	{width:566,	italic_font:0,	descender:0},
"&gt;":		{width:564,	italic_font:0,	descender:0},
"&le;":		{width:558,	italic_font:0,	descender:0},
"&ge;":		{width:558,	italic_font:0,	descender:0},
"&times;":	{width:565/*823*/,	italic_font:0,	descender:0},
"&minus;":	{width:565,	italic_font:0,	descender:0},
"&Gamma;":	{width:578,	italic_font:0,	descender:0},
"&Theta;":	{width:722,	italic_font:0,	descender:0},
"&Phi;":	{width:731,	italic_font:0,	descender:0},
"&Alpha;":	{width:732,	italic_font:0,	descender:0},
"&Beta;":	{width:667,	italic_font:0,	descender:0},
"&Delta;":	{width:643,	italic_font:0,	descender:0},
"&Epsilon;":	{width:611,	italic_font:0,	descender:0},
"&Zeta;":	{width:611,	italic_font:0,	descender:0},
"&Eta;":	{width:723,	italic_font:0,	descender:0},
"&Iota;":	{width:333,	italic_font:0,	descender:0},
"&Kappa;":	{width:751,	italic_font:0,	descender:0},
"&Lambda;":	{width:736,	italic_font:0,	descender:0},
"&Mu;":		{width:889,	italic_font:0,	descender:0},
"&Nu;":		{width:729,	italic_font:0,	descender:0},
"&Xi;":		{width:643,	italic_font:0,	descender:0},
"&Pi;":		{width:725,	italic_font:0,	descender:0},
"&Rho;":	{width:556,	italic_font:0,	descender:0},
"&Sigma;":	{width:582,	italic_font:0,	descender:0},
"&Tau;":	{width:611,	italic_font:0,	descender:0},
"&Upsilon;":	{width:729,	italic_font:0,	descender:0},
"&Chi;":	{width:731,	italic_font:0,	descender:0},
"&Psi;":	{width:748,	italic_font:0,	descender:0},
"&Omega;":	{width:743,	italic_font:0,	descender:0},
"&alpha;":	{width:525,	italic_font:1,	descender:0},
"&delta;":	{width:483,	italic_font:1,	descender:0},
"&epsilon;":	{width:412,	italic_font:1,	descender:0},
"&sigma;":	{width:548,	italic_font:1,	descender:0},
"&tau;":	{width:411,	italic_font:1,	descender:0},
"&phi;":	{width:553,	italic_font:1,	descender:1},
"&beta;":	{width:529,	italic_font:1,	descender:1},
"&gamma;":	{width:407,	italic_font:1,	descender:1},
"&zeta;":	{width:551,	italic_font:1,	descender:1},
"&eta;":	{width:496,	italic_font:1,	descender:1},
"&theta;":	{width:528,	italic_font:1,	descender:0},
"&iota;":	{width:278,	italic_font:1,	descender:0},
"&kappa;":	{width:515,	italic_font:1,	descender:0},
"&lambda;":	{width:433,	italic_font:1,	descender:0},
"&mu;":		{width:502,	italic_font:1,	descender:1},
"&nu;":		{width:463,	italic_font:1,	descender:0},
"&xi;":		{width:511,	italic_font:1,	descender:1},
"&rho;":	{width:480,	italic_font:1,	descender:1},
"&upsilon;":	{width:459,	italic_font:1,	descender:0},
"&chi;":	{width:490,	italic_font:1,	descender:1},
"&psi;":	{width:668,	italic_font:1,	descender:1},
"&omega;":	{width:706,	italic_font:1,	descender:0},
"&pi;":		{width:553,	italic_font:1,	descender:0},
"&hbar;":	{width:575/*515*/,	italic_font:0,	descender:0},
};
