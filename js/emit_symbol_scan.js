var emit_tab = [
	"Alpha","Beta","Gamma","Delta","Epsilon","Zeta","Eta","Theta","Iota",
	"Kappa","Lambda","Mu","Nu","Xi","Pi","Rho","Sigma","Tau","Upsilon",
	"Phi","Chi","Psi","Omega",
	"alpha","beta","gamma","delta","epsilon","zeta","eta","theta","iota",
	"kappa","lambda","mu","nu","xi","pi","rho","sigma","tau","upsilon",
	"phi","chi","psi","omega",
];

function
emit_symbol_scan(s, k)
{
	var i, n, t;
	n = emit_tab.length;
	for (i = 0; i < n; i++) {
		t = emit_tab[i];
		if (s.startsWith(t, k))
			return t.length;
	}
	return 1;
}
