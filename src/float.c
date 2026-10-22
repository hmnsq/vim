/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * float.c: Floating point functions
 */
#నిర్వచించు USING_FLOAT_STUFF

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

#నిర్వచితమైతే VMS
# include <float.h>
#అయితే_ముగింపు

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    పూర్ణాంకం
string2float(
    char_u	*పాఠ్యం,
    float_T	*విలువ,	    // result stored here
    పూర్ణాంకం		skip_quotes)
{
    అక్షరం	*s = (అక్షరం *)పాఠ్యం;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    ఒకవేళ (STRNICMP(పాఠ్యం, "inf", 3) == 0)
    {
	*విలువ = INFINITY;
	తిరిగి 3;
    }
    ఒకవేళ (STRNICMP(పాఠ్యం, "-inf", 4) == 0)
    {
	*విలువ = -INFINITY;
	తిరిగి 4;
    }
    ఒకవేళ (STRNICMP(పాఠ్యం, "nan", 3) == 0)
    {
	*విలువ = NAN;
	తిరిగి 3;
    }
    ఒకవేళ (skip_quotes && vim_strchr((char_u *)s, '\'') != ఖాళీ)
    {
	char_u	    buf[100];
	char_u	    *p;
	పూర్ణాంకం	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	కొరకు (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    ఒకవేళ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    ఒకవేళ (!vim_isdigit(*p))
		ఆపు;
	}
	s = (అక్షరం *)buf;
	f = strtod(s, &s);
	*విలువ = f;
	తిరిగి (పూర్ణాంకం)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *విలువ = f;
    తిరిగి (పూర్ణాంకం)((char_u *)s - పాఠ్యం);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    స్థిర పూర్ణాంకం
get_float_arg(typval_T *argvars, float_T *f)
{
    ఒకవేళ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	తిరిగి OK;
    }
    ఒకవేళ (argvars[0].v_type == VAR_సంఖ్య)
    {
	*f = (float_T)argvars[0].vval.v_సంఖ్య;
	తిరిగి OK;
    }
    emsg(_(e_సంఖ్య_or_float_required));
    తిరిగి FAIL;
}

/*
 * "abs(expr)" function
 */
    శూన్య
f_abs(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    లేకపోతే
    {
	varnumber_T	n;
	పూర్ణాంకం		error = FALSE;

	n = tv_get_సంఖ్య_chk(&argvars[0], &error);
	ఒకవేళ (error)
	    rettv->vval.v_సంఖ్య = -1;
	లేకపోతే ఒకవేళ (n > 0)
	    rettv->vval.v_సంఖ్య = n;
	లేకపోతే
	    rettv->vval.v_సంఖ్య = -n;
    }
}

/*
 * "acos()" function
 */
    శూన్య
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    శూన్య
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    శూన్య
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    శూన్య
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    శూన్య
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    శూన్య
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = కోజ్యా(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    శూన్య
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    శూన్య
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ఘాతాంకం(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    శూన్య
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (get_float_arg(argvars, &f) != OK)
	తిరిగి;

    ఒకవేళ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_సంఖ్య = -VARNUM_MAX;
    లేకపోతే ఒకవేళ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_సంఖ్య = VARNUM_MAX;
    లేకపోతే
	rettv->vval.v_సంఖ్య = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    శూన్య
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    శూన్య
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

#ఒకవేళ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    శూన్య
f_isinf(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_సంఖ్య = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    శూన్య
f_isnan(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#అయితే_ముగింపు

/*
 * "log()" function
 */
    శూన్య
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = సంవర్గమానం(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    శూన్య
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    శూన్య
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = ఘాతం(fx, fy);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_విడత(float_T f)
{
    తిరిగి f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    శూన్య
f_విడత(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_విడత(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    శూన్య
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = జ్యా(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    శూన్య
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    శూన్య
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = వర్గమూలం(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    శూన్య
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    పూర్ణాంకం     isneg;
    పూర్ణాంకం	    skip_quotes;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    ఒకవేళ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (శూన్య)string2float(p, &rettv->vval.v_float, skip_quotes);
    ఒకవేళ (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    శూన్య
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    శూన్య
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    శూన్య
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ఒకవేళ (లోపల_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_FLOAT;
    ఒకవేళ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    లేకపోతే
	rettv->vval.v_float = 0.0;
}

#అయితే_ముగింపు
