function
eval_incbeta(p1)
{
	var a, b, x, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("incbeta: 1st argument is not numerical");
	push(p2);
	x = pop_double();

	push(caddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("incbeta: 2nd argument is not numerical");
	push(p2);
	a = pop_double();

	push(cadddr(p1));
	evalf();
	p2 = pop();
	if (!isnum(p2))
		stopf("incbeta: 3rd argument is not numerical");
	push(p2);
	b = pop_double();

	if (x <= 0.0) {
		push_double(0.0);
		return;
	}

	if (x >= 1.0) {
		push_double(1.0);
		return;
	}

	x = incbeta(a, b, x);

	if (!isFinite(x))
		stopf("incbeta did not converge");

	push_double(x);
}

// https://github.com/codeplea/incbeta/blob/master/incbeta.c

/*
 * zlib License
 *
 * Regularized Incomplete Beta Function
 *
 * Copyright (c) 2016, 2017 Lewis Van Winkle
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */


//#include <math.h>

//#define STOPVAL 1.0e-8
//#define TINY 1.0e-30

function
incbeta(a, b, x)
{
    var STOPVAL = 1.0e-8;
    var TINY = 1.0e-30;

    if (x < 0.0 || x > 1.0) return 1.0/0.0;

    /*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
    if (x > (a+1.0)/(a+b+2.0)) {
        return (1.0-incbeta(b,a,1.0-x)); /*Use the fact that beta is symmetrical.*/
    }

    /*Find the first part before the continued fraction.*/
    var lbeta_ab = log_gamma(a)+log_gamma(b)-log_gamma(a+b);
    var front = Math.exp(Math.log(x)*a+Math.log(1.0-x)*b-lbeta_ab) / a;

    /*Use Lentz's algorithm to evaluate the continued fraction.*/
    var f = 1.0, c = 1.0, d = 0.0;

    var i, m;
    for (i = 0; i <= 200; ++i) {
        m = Math.floor(i/2);

        var numerator;
        if (i == 0) {
            numerator = 1.0; /*First numerator is 1.0.*/
        } else if (i % 2 == 0) {
            numerator = (m*(b-m)*x)/((a+2.0*m-1.0)*(a+2.0*m)); /*Even term.*/
        } else {
            numerator = -((a+m)*(a+b+m)*x)/((a+2.0*m)*(a+2.0*m+1)); /*Odd term.*/
        }

        /*Do an iteration of Lentz's algorithm.*/
        d = 1.0 + numerator * d;
        if (Math.abs(d) < TINY) d = TINY;
        d = 1.0 / d;

        c = 1.0 + numerator / c;
        if (Math.abs(c) < TINY) c = TINY;

        var cd = c*d;
        f *= cd;

        /*Check for stop.*/
        if (Math.abs(1.0-cd) < STOPVAL) {
            return front * (f-1.0);
        }
    }

    return 1.0/0.0; /*Needed more loops, did not converge.*/
}
