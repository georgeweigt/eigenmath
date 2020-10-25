const SPACE = 0;
const TEXT = 1;
const LINE = 2;
const PAREN = 3;
const SUPERSCRIPT = 4;
const SUBSCRIPT = 5;
const FRACTION = 6;
const TABLE = 7;

const FONT_SIZE = 20
const SMALL_FONT_SIZE = 14;

const HEIGHT_RATIO = 1.0;
const DEPTH_RATIO = 0.3;
const WIDTH_RATIO = 1 / 730;

const X_HEIGHT_RATIO = 0.3;
const V_SPACE_RATIO = 0.2;

const FONT_HEIGHT = HEIGHT_RATIO * FONT_SIZE;
const SMALL_FONT_HEIGHT = HEIGHT_RATIO * SMALL_FONT_SIZE;

const FONT_DEPTH = DEPTH_RATIO * FONT_SIZE;
const SMALL_FONT_DEPTH = DEPTH_RATIO * SMALL_FONT_SIZE;

const X_HEIGHT = X_HEIGHT_RATIO * FONT_SIZE;
const SMALL_X_HEIGHT = X_HEIGHT_RATIO * SMALL_FONT_SIZE;

const VSPACE = V_SPACE_RATIO * FONT_SIZE;
const SMALL_VSPACE = V_SPACE_RATIO * SMALL_FONT_SIZE;

const TABLE_HSPACE = 0.6 * FONT_SIZE;
const TABLE_VSPACE = 0.2 * FONT_SIZE;

const roman_width_tab = [
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
333,463,500,444,523,444,456,503,
515,278,278,527,278,794,517,500,
500,521,360,389,300,520,512,734,
509,515,449,480,200,480,563,778,
];

const italic_width_tab = [
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

const emit_stab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
	"hbar",
];

const symbol_width_tab = {
"&parenleft;":333,
"&parenright;":333,
"&plus;":565,
"&lt;":565,
"&equals;":566,
"&gt;":564,
"&le;":558,
"&ge;":558,
"&times;":823,
"&minus;":565,
"&Gamma;":578,
"&Theta;":722,
"&Phi;":731,
"&Alpha;":732,
"&Beta;":667,
"&Delta;":643,
"&Epsilon;":611,
"&Zeta;":611,
"&Eta;":723,
"&Iota;":333,
"&Kappa;":751,
"&Lambda;":736,
"&Mu;":889,
"&Nu;":729,
"&Xi;":643,
"&Pi;":725,
"&Rho;":556,
"&Sigma;":582,
"&Tau;":611,
"&Upsilon;":729,
"&Chi;":731,
"&Psi;":748,
"&Omega;":743,

"&alpha;":525,
"&delta;":483,
"&epsilon;":412,
"&sigma;":548,
"&tau;":411,
"&phi;":553,
"&hbar;":500,
"&beta;":529,
"&gamma;":407,
"&zeta;":551,
"&eta;":496,
"&theta;":528,
"&iota;":278,
"&kappa;":515,
"&lambda;":433,
"&mu;":502,
"&nu;":463,
"&xi;":511,
"&rho;":480,
"&upsilon;":459,
"&chi;":490,
"&psi;":668,
"&omega;":706,
"&pi;":553,
};
