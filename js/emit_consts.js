const SPACE = 0;
const ROMAN = 1;
const ITALIC = 2;
const LINE = 3;
const PAREN = 4;
const BRACK = 5;
const SUPERSCRIPT = 6;
const SUBSCRIPT = 7;
const FRACTION = 8;
const TABLE = 9;

const FONT_SIZE = 20
const SMALL_FONT_SIZE = 14;

const FONT_HEIGHT = Math.round(1.0 * FONT_SIZE);
const FONT_DEPTH = Math.round(0.3 * FONT_SIZE);
const FONT_WIDTH = Math.round(0.8 * FONT_SIZE);
const X_HEIGHT = Math.round(0.4 * FONT_SIZE);
const FRAC_PAD = Math.round(0.2 * FONT_SIZE);
const CELL_PAD = Math.round(0.2 * FONT_SIZE);

const SMALL_FONT_HEIGHT = Math.round(1.0 * SMALL_FONT_SIZE);
const SMALL_FONT_DEPTH = Math.round(0.3 * SMALL_FONT_SIZE);
const SMALL_FONT_WIDTH = Math.round(0.8 * SMALL_FONT_SIZE);
const SMALL_X_HEIGHT = Math.round(0.4 * SMALL_FONT_SIZE);
const SMALL_FRAC_PAD = Math.round(0.2 * SMALL_FONT_SIZE);

const emit_stab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
	"hbar",
];
