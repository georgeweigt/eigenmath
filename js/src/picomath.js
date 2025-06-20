// https://github.com/ghewgill/picomath

function
erf(x)
{
	if (x == 0)
		return 0;

	// constants
	var a1 = 0.254829592;
	var a2 = -0.284496736;
	var a3 = 1.421413741;
	var a4 = -1.453152027;
	var a5 = 1.061405429;
	var p = 0.3275911;

	// Save the sign of x
	var sign = 1;
	if (x < 0)
		sign = -1;
	x = Math.abs(x);

	// A&S formula 7.1.26
	var t = 1.0/(1.0 + p*x);
	var y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*Math.exp(-x*x);

	return sign*y;
}

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
        return Double.POSITIVE_INFINITY;
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
