void eval_exit(struct atom *p1);
void echo_input(char *s);
int check_display(void);
void prep_display(void);
void clear_display(void);
struct display * get_display_ptr(void);
void set_display_ptr(struct display *p);
void display(void);
void draw_display(double ymin, double ymax);
void draw_display_nib(struct display *p, double y, double ymin, double ymax);
void draw_text(double x, double y, uint8_t *buf, int len);
void draw_formula(double x, double y, double *p);
void draw_char(double x, double y, int font_num, int char_num);
void draw_stroke(double x1, double y1, double x2, double y2, double stroke_width);
void draw_point(double x, double y);
void draw_selection_rect(double x, double y, double width, double height);
struct atom * alloc_atom(void);
void alloc_block(void);
struct atom * alloc_vector(int nrow);
struct atom * alloc_matrix(int nrow, int ncol);
struct atom * alloc_tensor(int nelem);
struct atom * alloc_str(void);
void * alloc_mem(int n);
double mfloat(uint32_t *p);
void msetbit(uint32_t *x, uint32_t k);
void mclrbit(uint32_t *x, uint32_t k);
uint32_t * mscan(char *s);
char * mstr(uint32_t *u);
int mdivby1billion(uint32_t *u);
uint32_t * madd(uint32_t *u, uint32_t *v);
uint32_t * msub(uint32_t *u, uint32_t *v);
uint32_t * mmul(uint32_t *u, uint32_t *v);
uint32_t * mdiv(uint32_t *u, uint32_t *v);
uint32_t * mmod(uint32_t *u, uint32_t *v);
uint32_t * mpow(uint32_t *u, uint32_t *v);
void mshr(uint32_t *u);
int mcmp(uint32_t *u, uint32_t *v);
int meq(uint32_t *u, uint32_t *v);
uint32_t * mint(uint32_t n);
uint32_t * mnew(int n);
void mfree(uint32_t *u);
uint32_t * mcopy(uint32_t *u);
void mnorm(uint32_t *u);
uint32_t * mgcd(uint32_t *u, uint32_t *v);
uint32_t * mroot(uint32_t *a, uint32_t *n);
void list(int n);
void cons(void);
int lengthf(struct atom *p);
int findf(struct atom *p, struct atom *q);
int complexity(struct atom *p);
void sort(int n);
int sort_func(const void *p1, const void *p2);
int find_denominator(struct atom *p);
int count_denominators(struct atom *p);
int count_numerators(struct atom *p);
int cmp(struct atom *p1, struct atom *p2);
int cmp_numbers(struct atom *p1, struct atom *p2);
int cmp_rationals(struct atom *a, struct atom *b);
int cmp_tensors(struct atom *p1, struct atom *p2);
int relop(struct atom *p1);
void evalg(void);
void evalf(void);
void evalf_nib(struct atom *p1);
void evalp(void);
void eval_abs(struct atom *p1);
void absfunc(void);
void eval_add(struct atom *p1);
void add(void);
void add_terms(int n);
void flatten_terms(int h);
struct atom * combine_tensors(int h);
void add_tensors(void);
void combine_terms(int h);
int combine_terms_nib(int i, int j);
void sort_terms(int n);
int sort_terms_func(const void *q1, const void *q2);
int cmp_terms(struct atom *p1, struct atom *p2);
int simplify_terms(int h);
int isradicalterm(struct atom *p);
int is_imaginary_term(struct atom *p);
void add_numbers(struct atom *p1, struct atom *p2);
void add_rationals(struct atom *p1, struct atom *p2);
void add_integers(struct atom *p1, struct atom *p2);
void subtract(void);
void eval_adj(struct atom *p1);
void adj(void);
void eval_and(struct atom *p1);
void eval_arccos(struct atom *p1);
void arccos(void);
void eval_arccosh(struct atom *p1);
void arccosh(void);
void eval_arcsin(struct atom *p1);
void arcsin(void);
void eval_arcsinh(struct atom *p1);
void arcsinh(void);
void eval_arctan(struct atom *p1);
void arctan(void);
void arctan_numbers(struct atom *X, struct atom *Y);
void eval_arctanh(struct atom *p1);
void arctanh(void);
void eval_arg(struct atom *p1);
void arg(void);
void arg1(void);
void eval_binding(struct atom *p1);
void eval_ceiling(struct atom *p1);
void ceilingfunc(void);
void eval_check(struct atom *p1);
void eval_circexp(struct atom *p1);
void circexp(void);
void circexp_subst(void);
void eval_clear(struct atom *p1);
void eval_clock(struct atom *p1);
void clockfunc(void);
void eval_cofactor(struct atom *p1);
void eval_conj(struct atom *p1);
void conjfunc(void);
void conjfunc_subst(void);
void eval_contract(struct atom *p1);
void contract(void);
void eval_cos(struct atom *p1);
void cosfunc(void);
void cosfunc_sum(struct atom *p1);
void eval_cosh(struct atom *p1);
void coshfunc(void);
void eval_defint(struct atom *p1);
void eval_denominator(struct atom *p1);
void denominator(void);
void eval_derivative(struct atom *p1);
void derivative(void);
void d_scalar_scalar(struct atom *F, struct atom *X);
void dsum(struct atom *p1, struct atom *p2);
void dproduct(struct atom *p1, struct atom *p2);
void dpower(struct atom *F, struct atom *X);
void dlog(struct atom *p1, struct atom *p2);
void dd(struct atom *p1, struct atom *p2);
void dfunction(struct atom *p1, struct atom *p2);
void dsin(struct atom *p1, struct atom *p2);
void dcos(struct atom *p1, struct atom *p2);
void dtan(struct atom *p1, struct atom *p2);
void darcsin(struct atom *p1, struct atom *p2);
void darccos(struct atom *p1, struct atom *p2);
void darctan(struct atom *p1, struct atom *p2);
void dsinh(struct atom *p1, struct atom *p2);
void dcosh(struct atom *p1, struct atom *p2);
void dtanh(struct atom *p1, struct atom *p2);
void darcsinh(struct atom *p1, struct atom *p2);
void darccosh(struct atom *p1, struct atom *p2);
void darctanh(struct atom *p1, struct atom *p2);
void derf(struct atom *p1, struct atom *p2);
void derfc(struct atom *p1, struct atom *p2);
void d_tensor_tensor(struct atom *p1, struct atom *p2);
void d_scalar_tensor(struct atom *p1, struct atom *p2);
void d_tensor_scalar(struct atom *p1, struct atom *p2);
void eval_det(struct atom *p1);
void det(void);
void eval_dim(struct atom *p1);
void eval_do(struct atom *p1);
void eval_eigenvec(struct atom *p1);
void eigenvec(double *D, double *Q, int n);
int eigenvec_step(double *D, double *Q, int n);
void eigenvec_step_nib(double *D, double *Q, int n, int p, int q);
void eval_erf(struct atom *p1);
void erffunc(void);
void eval_erfc(struct atom *p1);
void erfcfunc(void);
void eval_eval(struct atom *p1);
void eval_exp(struct atom *p1);
void expfunc(void);
void eval_expcos(struct atom *p1);
void expcos(void);
void eval_expcosh(struct atom *p1);
void expcosh(void);
void eval_expsin(struct atom *p1);
void expsin(void);
void eval_expsinh(struct atom *p1);
void expsinh(void);
void eval_exptan(struct atom *p1);
void exptan(void);
void eval_exptanh(struct atom *p1);
void exptanh(void);
void eval_factorial(struct atom *p1);
void factorial(void);
void eval_float(struct atom *p1);
void floatfunc(void);
void floatfunc_subst(void);
void eval_floor(struct atom *p1);
void floorfunc(void);
void eval_for(struct atom *p1);
void eval_hadamard(struct atom *p1);
void hadamard(void);
void eval_imag(struct atom *p1);
void imag(void);
void eval_index(struct atom *p1);
void indexfunc(struct atom *T, int h);
void eval_infixform(struct atom *p1);
void print_infixform(struct atom *p);
void infixform_subexpr(struct atom *p);
void infixform_expr(struct atom *p);
void infixform_expr_nib(struct atom *p);
void infixform_term(struct atom *p);
void infixform_term_nib(struct atom *p);
void infixform_numerators(struct atom *p);
void infixform_denominators(struct atom *p);
void infixform_factor(struct atom *p);
void infixform_power(struct atom *p);
void infixform_reciprocal(struct atom *p);
void infixform_factorial(struct atom *p);
void infixform_index(struct atom *p);
void infixform_arglist(struct atom *p);
void infixform_rational(struct atom *p);
void infixform_double(struct atom *p);
void infixform_base(struct atom *p);
void infixform_numeric_base(struct atom *p);
void infixform_numeric_exponent(struct atom *p);
void infixform_tensor(struct atom *p);
void infixform_tensor_nib(struct atom *p, int d, int k);
void eval_inner(struct atom *p1);
void inner(void);
void eval_integral(struct atom *p1);
void integral(void);
void integral_nib(struct atom *F, struct atom *X);
void integral_lookup(int h, struct atom *F);
int integral_classify(struct atom *p);
int integral_search(int h, struct atom *F, char **table, int n);
int integral_search_nib(int h, struct atom *F, struct atom *I, struct atom *C);
void decomp(void);
void decomp_sum(struct atom *F, struct atom *X);
void decomp_product(struct atom *F, struct atom *X);
void partition_term(void);
void eval_inv(struct atom *p1);
void inv(void);
void eval_kronecker(struct atom *p1);
void kronecker(void);
void eval_log(struct atom *p1);
void logfunc(void);
void eval_mag(struct atom *p1);
void mag(void);
void mag_nib(void);
void eval_minor(struct atom *p1);
void eval_minormatrix(struct atom *p1);
void minormatrix(int row, int col);
void eval_mod(struct atom *p1);
void modfunc(void);
void mod_rationals(struct atom *p1, struct atom *p2);
void mod_integers(struct atom *p1, struct atom *p2);
void eval_multiply(struct atom *p1);
void multiply(void);
void multiply_factors(int n);
void flatten_factors(int h);
struct atom * multiply_tensor_factors(int h);
void multiply_scalar_factors(int h);
struct atom * combine_numerical_factors(int h, struct atom *COEF);
void combine_factors(int h);
int combine_factors_nib(int i, int j);
void sort_factors_provisional(int h);
int sort_factors_provisional_func(const void *q1, const void *q2);
int cmp_factors_provisional(struct atom *p1, struct atom *p2);
void normalize_power_factors(int h);
void expand_sum_factors(int h);
void sort_factors(int n);
int sort_factors_func(const void *q1, const void *q2);
int cmp_factors(struct atom *p1, struct atom *p2);
int order_factor(struct atom *p);
void multiply_numbers(struct atom *p1, struct atom *p2);
void multiply_rationals(struct atom *p1, struct atom *p2);
struct atom * reduce_radical_factors(int h, struct atom *COEF);
int any_radical_factors(int h);
struct atom * reduce_radical_double(int h, struct atom *COEF);
struct atom * reduce_radical_rational(int h, struct atom *COEF);
void multiply_expand(void);
void multiply_noexpand(void);
void multiply_factors_noexpand(int n);
void negate(void);
void negate_noexpand(void);
void reciprocate(void);
void divide(void);
void eval_nil(struct atom *p1);
void eval_noexpand(struct atom *p1);
void eval_not(struct atom *p1);
void eval_nroots(struct atom *p1);
void nroots(void);
void nfindroot(double cr[], double ci[], int n, double *par, double *pai);
void fata(double cr[], double ci[], int n, double ar, double ai, double *far, double *fai);
void nreduce(double cr[], double ci[], int n, double ar, double ai);
double zabs(double r, double i);
double urandom(void);
void eval_number(struct atom *p1);
void eval_numerator(struct atom *p1);
void numerator(void);
void eval_or(struct atom *p1);
void eval_outer(struct atom *p1);
void outer(void);
void eval_polar(struct atom *p1);
void polar(void);
void eval_power(struct atom *p1);
void power(void);
void power_numbers(struct atom *BASE, struct atom *EXPO);
void power_numbers_factor(struct atom *BASE, struct atom *EXPO);
void power_double(struct atom *BASE, struct atom *EXPO);
void power_minusone(struct atom *EXPO);
void normalize_clock_rational(struct atom *EXPO);
void normalize_clock_double(struct atom *EXPO);
void power_natural_number(struct atom *EXPO);
void normalize_polar(struct atom *EXPO);
void normalize_polar_term(struct atom *EXPO);
void normalize_polar_term_rational(struct atom *R);
void normalize_polar_term_double(struct atom *R);
void power_sum(struct atom *BASE, struct atom *EXPO);
void power_complex_number(struct atom *BASE, struct atom *EXPO);
void power_complex_plus(struct atom *X, struct atom *Y, int n);
void power_complex_minus(struct atom *X, struct atom *Y, int n);
void power_complex_double(struct atom *X, struct atom *Y, struct atom *EXPO);
void power_complex_rational(struct atom *X, struct atom *Y, struct atom *EXPO);
void eval_prefixform(struct atom *p1);
void print_prefixform(struct atom *p);
void prefixform(struct atom *p);
void eval_print(struct atom *p1);
void print_result(void);
int annotate_result(struct atom *p1, struct atom *p2);
void eval_product(struct atom *p1);
void eval_quote(struct atom *p1);
void eval_rank(struct atom *p1);
void eval_rationalize(struct atom *p1);
void rationalize(void);
int find_divisor(struct atom *p);
int find_divisor_term(struct atom *p);
int find_divisor_factor(struct atom *p);
void cancel_factor(void);
void eval_real(struct atom *p1);
void real(void);
void eval_rect(struct atom *p1);
void rect(void);
void eval_roots(struct atom *p1);
void roots(void);
int findroot(int h, int n);
void horner(int h, int n, struct atom *A);
void divisors(int n);
void divisors_nib(int h, int k);
void reduce(int h, int n, struct atom *A);
void coeffs(struct atom *P, struct atom *X);
void eval_rotate(struct atom *p1);
void rotate_h(struct atom *PSI, uint32_t c, int n);
void rotate_p(struct atom *PSI, struct atom *PHASE, uint32_t c, int n);
void rotate_w(struct atom *PSI, uint32_t c, int m, int n);
void rotate_x(struct atom *PSI, uint32_t c, int n);
void rotate_y(struct atom *PSI, uint32_t c, int n);
void rotate_z(struct atom *PSI, uint32_t c, int n);
void rotate_q(struct atom *PSI, int n);
void rotate_v(struct atom *PSI, int n);
void eval_run(struct atom *p1);
char * read_file(char *filename);
void eval_setq(struct atom *p1);
void setq_indexed(struct atom *p1);
void set_component(struct atom *LVAL, struct atom *RVAL, int h);
void setq_usrfunc(struct atom *p1);
void convert_body(struct atom *A);
void eval_sgn(struct atom *p1);
void sgn(void);
void eval_simplify(struct atom *p1);
void simplify(void);
void simplify_pass1(void);
void simplify_pass2(void);
void simplify_pass3(void);
void eval_sin(struct atom *p1);
void sinfunc(void);
void sinfunc_sum(struct atom *p1);
void eval_sinh(struct atom *p1);
void sinhfunc(void);
void eval_sqrt(struct atom *p1);
void sqrtfunc(void);
void eval_status(struct atom *p1);
void eval_stop(struct atom *p1);
void eval_subst(struct atom *p1);
void subst(void);
void eval_sum(struct atom *p1);
void eval_tan(struct atom *p1);
void tanfunc(void);
void tanfunc_sum(struct atom *p1);
void eval_tanh(struct atom *p1);
void tanhfunc(void);
void eval_taylor(struct atom *p1);
void eval_tensor(struct atom *p1);
void promote_tensor(void);
int compatible_dimensions(struct atom *p, struct atom *q);
struct atom * copy_tensor(struct atom *p1);
void eval_test(struct atom *p1);
void eval_testeq(struct atom *p1);
void eval_testge(struct atom *p1);
void eval_testgt(struct atom *p1);
void eval_testle(struct atom *p1);
void eval_testlt(struct atom *p1);
void eval_transpose(struct atom *p1);
void transpose(int n, int m);
void eval_unit(struct atom *p1);
void eval_user_function(struct atom *p1);
void eval_user_symbol(struct atom *p1);
void eval_zero(struct atom *p1);
void factor_factor(void);
void factor_bignum(uint32_t *N, struct atom *M);
void factor_int(int n);
void gc(void);
void untag(struct atom *p);
void outbuf_init(void);
void outbuf_puts(char *s);
void outbuf_putc(int c);
int iszero(struct atom *p);
int isequaln(struct atom *p, int n);
int isequalq(struct atom *p, int a, int b);
int isplusone(struct atom *p);
int isminusone(struct atom *p);
int isinteger(struct atom *p);
int isinteger1(struct atom *p);
int isfraction(struct atom *p);
int isposint(struct atom *p);
int iseveninteger(struct atom *p);
int isradical(struct atom *p);
int isnegativeterm(struct atom *p);
int isnegativenumber(struct atom *p);
int iscomplexnumber(struct atom *p);
int isimaginarynumber(struct atom *p);
int isimaginaryunit(struct atom *p);
int isoneoversqrttwo(struct atom *p);
int isminusoneoversqrttwo(struct atom *p);
int isdoublez(struct atom *p);
int isdenominator(struct atom *p);
int isnumerator(struct atom *p);
int isdoublesomewhere(struct atom *p);
int isusersymbolsomewhere(struct atom *p);
int isdenormalpolar(struct atom *p);
int isdenormalpolarterm(struct atom *p);
int issquarematrix(struct atom *p);
int issmallinteger(struct atom *p);
void run(char *buf);
void run_buf(char *buf);
char * scan_input(char *s);
void print_trace(int color);
void run_init_script(void);
void stopf(char *s);
void stopf_cond(char *s);
char * scan(char *s);
char * scan1(char *s);
char * scan_nib(char *s);
void scan_stmt(void);
void scan_comparison(void);
void scan_expression(void);
int another_factor_pending(void);
void scan_term(void);
void scan_power(void);
void scan_factor(void);
void scan_symbol(void);
void scan_string(void);
void scan_function_call(void);
void scan_integer(void);
void scan_subexpr(void);
void get_token_skip_newlines(void);
void get_token(void);
void get_token_nib(void);
void update_token_buf(char *a, char *b);
void scan_error(char *errmsg);
void build_tensor(int h);
void static_negate(void);
void static_reciprocate(void);
void push(struct atom *p);
struct atom * pop(void);
void fpush(struct atom *p);
struct atom * fpop(void);
void save_symbol(struct atom *p);
void restore_symbol(void);
void dupl(void);
void swap(void);
void push_integer(int n);
void push_rational(int a, int b);
void push_bignum(int sign, uint32_t *a, uint32_t *b);
int pop_integer(void);
void push_double(double d);
double pop_double(void);
void push_string(char *s);
struct atom * lookup(char *s);
char * printname(struct atom *p);
void set_symbol(struct atom *p1, struct atom *p2, struct atom *p3);
struct atom * get_binding(struct atom *p1);
struct atom * get_usrfunc(struct atom *p);
void init_symbol_table(void);
void emit_args(struct atom *p);
void emit_base(struct atom *p);
void emit_box(void);
void emit_denominators(struct atom *p);
void emit_double(struct atom *p);
void emit_exponent(struct atom *p);
void emit_expr(struct atom *p);
void emit_expr_nib(struct atom *p);
void emit_factor(struct atom *p);
void emit_formula(double x, double y, struct atom *p);
void emit_formula_delims(double x, double y, double h, double d, double w, double stroke_width, int font_num);
void emit_formula_ldelim(double x, double y, double h, double d, double w, double stroke_width);
void emit_formula_rdelim(double x, double y, double h, double d, double w, double stroke_width);
void emit_formula_fraction(double x, double y, double h, double d, double w, double stroke_width, int font_num, struct atom *p);
void emit_formula_table(double x, double y, struct atom *p);
void emit_formula_char(double x, double y, int font_num, int char_num);
void emit_formula_stroke(double x1, double y1, double x2, double y2, double stroke_width);
void emit_frac(struct atom *p);
void emit_function(struct atom *p);
void emit_graph(void);
void emit_indices(struct atom *p);
void emit_infix_operator(int c);
void emit_italic_char(int char_num);
void emit_italic_string(char *s);
void emit_labels(void);
void emit_list(struct atom *p);
void emit_matrix(struct atom *p, int d, int k);
void emit_medium_space(void);
void emit_numerators(struct atom *p);
void emit_numeric_exponent(struct atom *p);
void emit_power(struct atom *p);
void emit_push(double d);
void emit_rational(struct atom *p);
void emit_reciprocal(struct atom *p);
void emit_roman_char(int char_num);
void emit_roman_string(char *s);
void emit_string(struct atom *p);
void emit_subexpr(struct atom *p);
void emit_symbol(struct atom *p);
int emit_symbol_fragment(char *s, int k);
void emit_tensor(struct atom *p);
void emit_term(struct atom *p);
void emit_term_nib(struct atom *p);
void emit_text(char *buf, int len, int color);
void emit_thick_space(void);
void emit_thin_space(void);
void emit_update_fraction(void);
void emit_update_list(int t);
void emit_update_subexpr(void);
void emit_update_subscript(void);
void emit_update_superscript(void);
void emit_update_table(int n, int m);
void emit_vector(struct atom *p);
void emit_xaxis(void);
void emit_yaxis(void);
void eval_draw(struct atom *p1);
void setup_trange(void);
void setup_xrange(void);
void setup_yrange(void);
void setup_final(struct atom *F, struct atom *T);
void draw_pass1(struct atom *F, struct atom *T);
void draw_pass2(struct atom *F, struct atom *T);
void sample(struct atom *F, struct atom *T, double t);
int inrange(double x, double y);
void eval_nonstop(void);
void eval_nonstop_nib(void);
void init_fonts(void);
double get_ascent(int font_num);
double get_cap_height(int font_num);
double get_char_depth(int font_num, int char_num);
CFStringRef get_char_name(int char_num);
double get_char_width(int font_num, int char_num);
double get_descent(int font_num);
CTFontRef get_font_ref(int font_num);
double get_leading(int font_num);
double get_xheight(int font_num);
double get_operator_height(int font_num);
char * history_up(char *instring);
char * history_down(char *instring);
void history_push(char *instring);
void printbuf(char *s, int color);
void run_thread(char *s);
void * run_thread_nib(void *p);
void shipout(struct display *p);
