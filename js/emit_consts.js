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

const FONT_HEIGHT = Math.round(1.0 * FONT_SIZE);
const SMALL_FONT_HEIGHT = Math.round(1.0 * SMALL_FONT_SIZE);

const FONT_DEPTH = Math.round(0.3 * FONT_SIZE);
const SMALL_FONT_DEPTH = Math.round(0.3 * SMALL_FONT_SIZE);

const FONT_WIDTH = Math.round(0.6 * FONT_SIZE);
const SMALL_FONT_WIDTH = Math.round(0.6 * SMALL_FONT_SIZE);

const X_HEIGHT = Math.round(0.4 * FONT_SIZE);
const SMALL_X_HEIGHT = Math.round(0.4 * SMALL_FONT_SIZE);

const VSPACE = Math.round(0.2 * FONT_SIZE);
const SMALL_VSPACE = Math.round(0.2 * SMALL_FONT_SIZE);

const THIN_SPACE = Math.round(1/3 * FONT_WIDTH);
const SMALL_THIN_SPACE = Math.round(1/3 * SMALL_FONT_WIDTH);

const MEDIUM_SPACE = Math.round(2/3 * FONT_WIDTH);
const SMALL_MEDIUM_SPACE = Math.round(2/3 * SMALL_FONT_WIDTH);

const CELL_PAD = Math.round(0.2 * FONT_SIZE);

// 1.0 = width of '0'

const emit_wtab = [
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

//		!	"	#	$	%	&	'
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

//	(	)	*	+	,	-	.	/
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	0.6,	1.0,

//	0	1	2	3	4	5	6	7
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

//	8	9	:	;	<	=	>	?
	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,

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
	1.0,	0.6,	0.6,	1.0,	0.6,	1.4,	1.0,	1.0,

//	p	q	r	s	t	u	v	w
	1.0,	1.0,	0.7,	0.8,	0.6,	1.0,	1.0,	1.3,

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
