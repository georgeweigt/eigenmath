const SPACE = 0;
const TEXT = 1;
const LINE = 2;
const PAREN = 3;
const BRACK = 4;
const SUPERSCRIPT = 5;
const SUBSCRIPT = 6;
const FRACTION = 7;
const TABLE = 8;

const FONT_SIZE = 20
const SMALL_FONT_SIZE = 14;

const HEIGHT_RATIO = 1.0;
const DEPTH_RATIO = 0.3;
const WIDTH_RATIO = 0.7;

const X_HEIGHT_RATIO = 0.3;
const V_SPACE_RATIO = 0.2;

const THIN_SPACE_RATIO = 1/3 * WIDTH_RATIO;
const MEDIUM_SPACE_RATIO = 2/3 * WIDTH_RATIO;

const X_HEIGHT = emit_round(X_HEIGHT_RATIO * FONT_SIZE);
const SMALL_X_HEIGHT = emit_round(X_HEIGHT_RATIO * SMALL_FONT_SIZE);

const VSPACE = emit_round(V_SPACE_RATIO * FONT_SIZE);
const SMALL_VSPACE = emit_round(V_SPACE_RATIO * SMALL_FONT_SIZE);

const MEDIUM_SPACE = emit_round(MEDIUM_SPACE_RATIO * FONT_SIZE);
const SMALL_MEDIUM_SPACE = emit_round(MEDIUM_SPACE_RATIO * SMALL_FONT_SIZE);

const CELL_PAD = emit_round(0.2 * FONT_SIZE);

const emit_wtab = [
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

//	space	!	"	#	$	%	&	'
	0.7,	0.6,	0.6,	1.0,	1.0,	1.0,	1.0,	1.0,

//	(	)	*	+	,	-	.	/
	0.7,	0.7,	1.0,	1.0,	0.6,	1.0,	0.6,	1.0,

//	0	1	2	3	4	5	6	7
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

//	8	9	:	;	<	=	>	?
	1.0,	1.0,	0.6,	0.6,	1.0,	1.0,	1.0,	1.0,

//	@	A	B	C	D	E	F	G
	1.0,	1.5,	1.3,	1.4,	1.5,	1.3,	1.3,	1.5,

//	H	I	J	K	L	M	N	O
	1.5,	0.8,	0.9,	1.4,	1.3,	1.8,	1.3,	1.3,

//	P	Q	R	S	T	U	V	W
	1.3,	1.3,	1.3,	1.3,	1.3,	1.3,	1.3,	1.7,

//	X	Y	Z	[	\	]	^	_
	1.3,	1.3,	1.3,	1.0,	1.0,	1.0,	1.0,	1.0,

//	`	a	b	c	d	e	f	g
	1.0,	0.9,	0.9,	0.8,	1.0,	0.8,	0.8,	1.0,

//	h	i	j	k	l	m	n	o
	1.0,	0.5,	0.5,	1.0,	0.5,	1.4,	1.0,	1.0,

//	p	q	r	s	t	u	v	w
	1.0,	1.0,	0.7,	0.8,	0.6,	1.0,	1.0,	1.4,

//	x	y	z	{	|	}	~
	0.9,	0.9,	0.9,	1.0,	1.0,	1.0,	1.0,	1.0,
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

const emit_swtab = {
"Alpha"		: 1.5,
"Beta"		: 1.5,
"Gamma"		: 1.5,
"Delta"		: 1.5,
"Epsilon"	: 1.5,
"Zeta"		: 1.5,
"Eta"		: 1.5,
"Theta"		: 1.5,
"Iota"		: 1.5,
"Kappa"		: 1.5,
"Lambda"	: 1.5,
"Mu"		: 1.5,
"Nu"		: 1.5,
"Xi"		: 1.5,
"Pi"		: 1.5,
"Rho"		: 1.5,
"Sigma"		: 1.5,
"Tau"		: 1.5,
"Upsilon"	: 1.5,
"Phi"		: 1.5,
"Chi"		: 1.5,
"Psi"		: 1.5,
"Omega"		: 1.5,
"alpha"		: 1.1,
"beta"		: 1.0,
"gamma"		: 1.0,
"delta"		: 1.0,
"epsilon"	: 1.0,
"zeta"		: 1.0,
"eta"		: 1.0,
"theta"		: 1.0,
"iota"		: 1.0,
"kappa"		: 1.0,
"lambda"	: 1.0,
"mu"		: 1.0,
"nu"		: 1.0,
"xi"		: 1.0,
"pi"		: 1.0,
"rho"		: 1.0,
"sigma"		: 1.0,
"tau"		: 1.0,
"upsilon"	: 1.0,
"phi"		: 1.0,
"chi"		: 1.0,
"psi"		: 1.0,
"omega"		: 1.0,

"plus"		: 1.4,
"minus"		: 1.4,
"times"		: 1.4,
"equals"	: 1.4,
"ge"		: 1.4,
"gt"		: 1.4,
"le"		: 1.4,
"lt"		: 1.4,

"hbar"		: 1.0,
};
