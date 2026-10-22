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
#define USING_FLOAT_STUFF

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    முழு
string2float(
    char_u	*text,
    float_T	*மதிப்பு,	    // result stored here
    முழு		skip_quotes)
{
    எழுத்து	*s = (எழுத்து *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    எனில் (STRNICMP(text, "inf", 3) == 0)
    {
	*மதிப்பு = INFINITY;
	திரும்பு 3;
    }
    எனில் (STRNICMP(text, "-inf", 4) == 0)
    {
	*மதிப்பு = -INFINITY;
	திரும்பு 4;
    }
    எனில் (STRNICMP(text, "nan", 3) == 0)
    {
	*மதிப்பு = NAN;
	திரும்பு 3;
    }
    எனில் (skip_quotes && vim_strchr((char_u *)s, '\'') != வெற்று)
    {
	char_u	    buf[100];
	char_u	    *p;
	முழு	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	க்கு (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    எனில் (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    எனில் (!vim_isdigit(*p))
		நிறுத்து;
	}
	s = (எழுத்து *)buf;
	f = strtod(s, &s);
	*மதிப்பு = f;
	திரும்பு (முழு)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *மதிப்பு = f;
    திரும்பு (முழு)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    நிலையான முழு
get_float_arg(typval_T *argvars, float_T *f)
{
    எனில் (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	திரும்பு OK;
    }
    எனில் (argvars[0].v_type == VAR_எண்)
    {
	*f = (float_T)argvars[0].vval.v_எண்;
	திரும்பு OK;
    }
    emsg(_(e_எண்_or_float_required));
    திரும்பு FAIL;
}

/*
 * "abs(expr)" function
 */
    வெறுமை
f_abs(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    இல்லையெனில்
    {
	varnumber_T	n;
	முழு		error = FALSE;

	n = tv_get_எண்_chk(&argvars[0], &error);
	எனில் (error)
	    rettv->vval.v_எண் = -1;
	இல்லையெனில் எனில் (n > 0)
	    rettv->vval.v_எண் = n;
	இல்லையெனில்
	    rettv->vval.v_எண் = -n;
    }
}

/*
 * "acos()" function
 */
    வெறுமை
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    வெறுமை
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    வெறுமை
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    வெறுமை
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    எனில் (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    வெறுமை
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    வெறுமை
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    வெறுமை
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    வெறுமை
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    வெறுமை
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (get_float_arg(argvars, &f) != OK)
	திரும்பு;

    எனில் (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_எண் = -VARNUM_MAX;
    இல்லையெனில் எனில் (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_எண் = VARNUM_MAX;
    இல்லையெனில்
	rettv->vval.v_எண் = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    வெறுமை
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    வெறுமை
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    எனில் (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

#எனில் defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    வெறுமை
f_isinf(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_எண் = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    வெறுமை
f_isnan(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    வெறுமை
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    வெறுமை
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    வெறுமை
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    எனில் (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    திரும்பு f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    வெறுமை
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    வெறுமை
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    வெறுமை
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    வெறுமை
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    வெறுமை
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    முழு     isneg;
    முழு	    skip_quotes;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    எனில் (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (வெறுமை)string2float(p, &rettv->vval.v_float, skip_quotes);
    எனில் (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    வெறுமை
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    வெறுமை
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    வெறுமை
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    எனில் (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->v_type = VAR_FLOAT;
    எனில் (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    இல்லையெனில்
	rettv->vval.v_float = 0.0;
}

#endif
