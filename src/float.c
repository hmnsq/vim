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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    صحیح
string2float(
    char_u	*text,
    float_T	*مقدار,	    // result stored here
    صحیح		skip_quotes)
{
    نویسه	*s = (نویسه *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    اگر (STRNICMP(text, "inf", 3) == 0)
    {
	*مقدار = INFINITY;
	بازگشت 3;
    }
    اگر (STRNICMP(text, "-inf", 4) == 0)
    {
	*مقدار = -INFINITY;
	بازگشت 4;
    }
    اگر (STRNICMP(text, "nan", 3) == 0)
    {
	*مقدار = NAN;
	بازگشت 3;
    }
    اگر (skip_quotes && vim_strchr((char_u *)s, '\'') != پوچ)
    {
	char_u	    buf[100];
	char_u	    *p;
	صحیح	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	برای (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    اگر (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    اگر (!vim_isdigit(*p))
		بشکن;
	}
	s = (نویسه *)buf;
	f = strtod(s, &s);
	*مقدار = f;
	بازگشت (صحیح)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *مقدار = f;
    بازگشت (صحیح)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ایستا صحیح
get_float_arg(typval_T *argvars, float_T *f)
{
    اگر (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	بازگشت OK;
    }
    اگر (argvars[0].v_type == VAR_عدد)
    {
	*f = (float_T)argvars[0].vval.v_عدد;
	بازگشت OK;
    }
    emsg(_(e_عدد_or_float_required));
    بازگشت FAIL;
}

/*
 * "abs(expr)" function
 */
    تهی
f_abs(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    وگرنه
    {
	varnumber_T	n;
	صحیح		error = FALSE;

	n = tv_get_عدد_chk(&argvars[0], &error);
	اگر (error)
	    rettv->vval.v_عدد = -1;
	وگرنه اگر (n > 0)
	    rettv->vval.v_عدد = n;
	وگرنه
	    rettv->vval.v_عدد = -n;
    }
}

/*
 * "acos()" function
 */
    تهی
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    تهی
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    تهی
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    تهی
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    تهی
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    تهی
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    تهی
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    تهی
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    تهی
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (get_float_arg(argvars, &f) != OK)
	بازگشت;

    اگر (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_عدد = -VARNUM_MAX;
    وگرنه اگر (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_عدد = VARNUM_MAX;
    وگرنه
	rettv->vval.v_عدد = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    تهی
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    تهی
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    وگرنه
	rettv->vval.v_float = 0.0;
}

#اگر defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    تهی
f_isinf(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_عدد = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    تهی
f_isnan(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    تهی
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    تهی
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    تهی
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    وگرنه
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    بازگشت f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    تهی
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    تهی
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    تهی
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    تهی
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    تهی
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    صحیح     isneg;
    صحیح	    skip_quotes;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    اگر (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (تهی)string2float(p, &rettv->vval.v_float, skip_quotes);
    اگر (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    تهی
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    تهی
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    تهی
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    وگرنه
	rettv->vval.v_float = 0.0;
}

#endif
