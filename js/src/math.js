// https://github.com/josdejong/mathjs

function
erf(x)
{
	var y = Math.abs(x);
	if (y >= MAX_NUM)
		return Math.sign(x);
	if (y <= THRESH)
		return Math.sign(x) * erf1(y);
	if (y <= 4.0)
		return Math.sign(x) * (1 - erfc2(y));
	return Math.sign(x) * (1 - erfc3(y));
}

  /**
   * Approximates the error function erf() for x <= 0.46875 using this function:
   *               n
   * erf(x) = x * sum (p_j * x^(2j)) / (q_j * x^(2j))
   *              j=0
   */
  function erf1 (y) {
    const ysq = y * y
    let xnum = P[0][4] * ysq
    let xden = ysq
    let i

    for (i = 0; i < 3; i += 1) {
      xnum = (xnum + P[0][i]) * ysq
      xden = (xden + Q[0][i]) * ysq
    }
    return y * (xnum + P[0][3]) / (xden + Q[0][3])
  }

  /**
   * Approximates the complement of the error function erfc() for
   * 0.46875 <= x <= 4.0 using this function:
   *                       n
   * erfc(x) = e^(-x^2) * sum (p_j * x^j) / (q_j * x^j)
   *                      j=0
   */
  function erfc2 (y) {
    let xnum = P[1][8] * y
    let xden = y
    let i

    for (i = 0; i < 7; i += 1) {
      xnum = (xnum + P[1][i]) * y
      xden = (xden + Q[1][i]) * y
    }
    const result = (xnum + P[1][7]) / (xden + Q[1][7])
    const ysq = parseInt(y * 16) / 16
    const del = (y - ysq) * (y + ysq)
    return Math.exp(-ysq * ysq) * Math.exp(-del) * result
  }

  /**
   * Approximates the complement of the error function erfc() for x > 4.0 using
   * this function:
   *
   * erfc(x) = (e^(-x^2) / x) * [ 1/sqrt(pi) +
   *               n
   *    1/(x^2) * sum (p_j * x^(-2j)) / (q_j * x^(-2j)) ]
   *              j=0
   */
  function erfc3 (y) {
    let ysq = 1 / (y * y)
    let xnum = P[2][5] * ysq
    let xden = ysq
    let i

    for (i = 0; i < 4; i += 1) {
      xnum = (xnum + P[2][i]) * ysq
      xden = (xden + Q[2][i]) * ysq
    }
    let result = ysq * (xnum + P[2][4]) / (xden + Q[2][4])
    result = (SQRPI - result) / y
    ysq = parseInt(y * 16) / 16
    const del = (y - ysq) * (y + ysq)
    return Math.exp(-ysq * ysq) * Math.exp(-del) * result
  }

/**
 * Upper bound for the first approximation interval, 0 <= x <= THRESH
 * @constant
 */
const THRESH = 0.46875

/**
 * Constant used by W. J. Cody's Fortran77 implementation to denote sqrt(pi)
 * @constant
 */
const SQRPI = 5.6418958354775628695e-1

/**
 * Coefficients for each term of the numerator sum (p_j) for each approximation
 * interval (see W. J. Cody's paper for more details)
 * @constant
 */
const P = [[
  3.16112374387056560e00, 1.13864154151050156e02,
  3.77485237685302021e02, 3.20937758913846947e03,
  1.85777706184603153e-1
], [
  5.64188496988670089e-1, 8.88314979438837594e00,
  6.61191906371416295e01, 2.98635138197400131e02,
  8.81952221241769090e02, 1.71204761263407058e03,
  2.05107837782607147e03, 1.23033935479799725e03,
  2.15311535474403846e-8
], [
  3.05326634961232344e-1, 3.60344899949804439e-1,
  1.25781726111229246e-1, 1.60837851487422766e-2,
  6.58749161529837803e-4, 1.63153871373020978e-2
]]

/**
 * Coefficients for each term of the denominator sum (q_j) for each approximation
 * interval (see W. J. Cody's paper for more details)
 * @constant
 */
const Q = [[
  2.36012909523441209e01, 2.44024637934444173e02,
  1.28261652607737228e03, 2.84423683343917062e03
], [
  1.57449261107098347e01, 1.17693950891312499e02,
  5.37181101862009858e02, 1.62138957456669019e03,
  3.29079923573345963e03, 4.36261909014324716e03,
  3.43936767414372164e03, 1.23033935480374942e03
], [
  2.56852019228982242e00, 1.87295284992346047e00,
  5.27905102951428412e-1, 6.05183413124413191e-2,
  2.33520497626869185e-3
]]

/**
 * Maximum/minimum safe numbers to input to erf() (in ES6+, this number is
 * Number.[MAX|MIN]_SAFE_INTEGER). erf() for all numbers beyond this limit will
 * return 1
 */
const MAX_NUM = Math.pow(2, 53)

function
erfc(x)
{
	return 1.0 - erf(x);
}

// Visit http://www.johndcook.com/stand_alone_code.html for the source of this code and more like it.

// Note that the functions Gamma and LogGamma are mutually dependent.

function gamma
(
    x    // We require x > 0
)
{
    //if (x <= 0.0)
    //{
    //    String msg = String.format("Invalid input argument {0}. Argument must be positive.", x);
    //    throw new IllegalArgumentException(msg);
    //}

    // Split the function domain into three intervals:
    // (0, 0.001), [0.001, 12), and (12, infinity)

    ///////////////////////////////////////////////////////////////////////////
    // First interval: (0, 0.001)
    //
    // For small x, 1/Gamma(x) has power series x + gamma x^2  - ...
    // So in this range, 1/Gamma(x) = x + gamma x^2 with error on the order of x^3.
    // The relative error over this interval is less than 6e-7.

    var gamma = 0.577215664901532860606512090; // Euler's gamma constant

    if (x < 0.001)
        return 1.0/(x*(1.0 + gamma*x));

    ///////////////////////////////////////////////////////////////////////////
    // Second interval: [0.001, 12)

    if (x < 12.0)
    {
        // The algorithm directly approximates gamma over (1,2) and uses
        // reduction identities to reduce other arguments to this interval.
        
        var y = x;
        var n = 0;
        var arg_was_less_than_one = (y < 1.0);

        // Add or subtract integers as necessary to bring y into (1,2)
        // Will correct for this below
        if (arg_was_less_than_one)
        {
            y += 1.0;
        }
        else
        {
            n = Math.floor(y) - 1;  // will use n later
            y -= n;
        }

        // numerator coefficients for approximation over the interval (1,2)
        var p =
        [
            -1.71618513886549492533811E+0,
             2.47656508055759199108314E+1,
            -3.79804256470945635097577E+2,
             6.29331155312818442661052E+2,
             8.66966202790413211295064E+2,
            -3.14512729688483675254357E+4,
            -3.61444134186911729807069E+4,
             6.64561438202405440627855E+4
        ];

        // denominator coefficients for approximation over the interval (1,2)
        var q =
        [
            -3.08402300119738975254353E+1,
             3.15350626979604161529144E+2,
            -1.01515636749021914166146E+3,
            -3.10777167157231109440444E+3,
             2.25381184209801510330112E+4,
             4.75584627752788110767815E+3,
            -1.34659959864969306392456E+5,
            -1.15132259675553483497211E+5
        ];

        var num = 0.0;
        var den = 1.0;

        var z = y - 1;
	var i;
        for (i = 0; i < 8; i++)
        {
            num = (num + p[i])*z;
            den = den*z + q[i];
        }
        var result = num/den + 1.0;

        // Apply correction if argument was not initially in (1,2)
        if (arg_was_less_than_one)
        {
            // Use identity gamma(z) = gamma(z+1)/z
            // The variable "result" now holds gamma of the original y + 1
            // Thus we use y-1 to get back the orginal y.
            result /= (y-1.0);
        }
        else
        {
            // Use the identity gamma(z+n) = z*(z+1)* ... *(z+n-1)*gamma(z)
            for (i = 0; i < n; i++)
                result *= y++;
        }

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Third interval: [12, infinity)

    if (x > 171.624)
    {
        // Correct answer too large to display. 
//        return Double.POSITIVE_INFINITY;
	return Infinity;
    }

    return Math.exp(log_gamma(x));
}

function log_gamma
(
    x    // x must be positive
)
{
    //if (x <= 0.0)
    //{
    //        String msg = String.format("Invalid input argument {0}. Argument must be positive.", x);
    //throw new IllegalArgumentException(msg);
    //}

    if (x < 12.0)
    {
        return Math.log(Math.abs(gamma(x)));
    }

    // Abramowitz and Stegun 6.1.41
    // Asymptotic series should be good to at least 11 or 12 figures
    // For error analysis, see Whittiker and Watson
    // A Course in Modern Analysis (1927), page 252

    var c =
    [
         1.0/12.0,
        -1.0/360.0,
         1.0/1260.0,
        -1.0/1680.0,
         1.0/1188.0,
        -691.0/360360.0,
         1.0/156.0,
        -3617.0/122400.0
    ];
    var z = 1.0/(x*x);
    var sum = c[7];
    for (var i=6; i >= 0; i--)
    {
        sum *= z;
        sum += c[i];
    }
    var series = sum/x;

    var halfLogTwoPi = 0.91893853320467274178032973640562;
    var logGamma = (x - 0.5)*Math.log(x) - x + halfLogTwoPi + series;    
    return logGamma;
}
