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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
string2float(
    char_u	*text,
    float_T	*ମୂଲ୍ୟ,	    // result stored here
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_quotes)
{
    ଅକ୍ଷର	*s = (ଅକ୍ଷର *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    ଯଦି (STRNICMP(text, "inf", 3) == 0)
    {
	*ମୂଲ୍ୟ = INFINITY;
	ଫେରନ୍ତୁ 3;
    }
    ଯଦି (STRNICMP(text, "-inf", 4) == 0)
    {
	*ମୂଲ୍ୟ = -INFINITY;
	ଫେରନ୍ତୁ 4;
    }
    ଯଦି (STRNICMP(text, "nan", 3) == 0)
    {
	*ମୂଲ୍ୟ = NAN;
	ଫେରନ୍ତୁ 3;
    }
    ଯଦି (skip_quotes && vim_strchr((char_u *)s, '\'') != ଖାଲି)
    {
	char_u	    buf[100];
	char_u	    *p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	ପାଇଁ (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    ଯଦି (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    ଯଦି (!vim_isdigit(*p))
		ଭାଙ୍ଗ;
	}
	s = (ଅକ୍ଷର *)buf;
	f = strtod(s, &s);
	*ମୂଲ୍ୟ = f;
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *ମୂଲ୍ୟ = f;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_float_arg(typval_T *argvars, float_T *f)
{
    ଯଦି (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (argvars[0].v_type == VAR_ସଂଖ୍ୟା)
    {
	*f = (float_T)argvars[0].vval.v_ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ OK;
    }
    emsg(_(e_ସଂଖ୍ୟା_or_float_required));
    ଫେରନ୍ତୁ FAIL;
}

/*
 * "abs(expr)" function
 */
    ଶୂନ୍ୟ
f_abs(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    ଅନ୍ୟଥା
    {
	varnumber_T	n;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	n = tv_get_ସଂଖ୍ୟା_chk(&argvars[0], &error);
	ଯଦି (error)
	    rettv->vval.v_ସଂଖ୍ୟା = -1;
	ଅନ୍ୟଥା ଯଦି (n > 0)
	    rettv->vval.v_ସଂଖ୍ୟା = n;
	ଅନ୍ୟଥା
	    rettv->vval.v_ସଂଖ୍ୟା = -n;
    }
}

/*
 * "acos()" function
 */
    ଶୂନ୍ୟ
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    ଶୂନ୍ୟ
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    ଶୂନ୍ୟ
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    ଶୂନ୍ୟ
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ଯଦି (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    ଶୂନ୍ୟ
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    ଶୂନ୍ୟ
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    ଶୂନ୍ୟ
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    ଶୂନ୍ୟ
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    ଶୂନ୍ୟ
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (get_float_arg(argvars, &f) != OK)
	ଫେରନ୍ତୁ;

    ଯଦି (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_ସଂଖ୍ୟା = -VARNUM_MAX;
    ଅନ୍ୟଥା ଯଦି (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_ସଂଖ୍ୟା = VARNUM_MAX;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    ଶୂନ୍ୟ
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    ଶୂନ୍ୟ
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ଯଦି (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

#ଯଦି defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    ଶୂନ୍ୟ
f_isinf(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_ସଂଖ୍ୟା = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    ଶୂନ୍ୟ
f_isnan(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    ଶୂନ୍ୟ
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    ଶୂନ୍ୟ
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    ଶୂନ୍ୟ
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ଯଦି (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    ଫେରନ୍ତୁ f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    ଶୂନ୍ୟ
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    ଶୂନ୍ୟ
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    ଶୂନ୍ୟ
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    ଶୂନ୍ୟ
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    ଶୂନ୍ୟ
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     isneg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip_quotes;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    ଯଦି (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (ଶୂନ୍ୟ)string2float(p, &rettv->vval.v_float, skip_quotes);
    ଯଦି (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    ଶୂନ୍ୟ
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    ଶୂନ୍ୟ
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    ଶୂନ୍ୟ
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ଯଦି (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_FLOAT;
    ଯଦି (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    ଅନ୍ୟଥା
	rettv->vval.v_float = 0.0;
}

#endif
