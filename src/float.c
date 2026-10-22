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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    પૂર્ણાંક
string2float(
    char_u	*text,
    float_T	*કિંમત,	    // result stored here
    પૂર્ણાંક		skip_quotes)
{
    અક્ષર	*s = (અક્ષર *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    જો (STRNICMP(text, "inf", 3) == 0)
    {
	*કિંમત = INFINITY;
	પાછા 3;
    }
    જો (STRNICMP(text, "-inf", 4) == 0)
    {
	*કિંમત = -INFINITY;
	પાછા 4;
    }
    જો (STRNICMP(text, "nan", 3) == 0)
    {
	*કિંમત = NAN;
	પાછા 3;
    }
    જો (skip_quotes && vim_strchr((char_u *)s, '\'') != ખાલી)
    {
	char_u	    buf[100];
	char_u	    *p;
	પૂર્ણાંક	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	માટે (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    જો (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    જો (!vim_isdigit(*p))
		તોડો;
	}
	s = (અક્ષર *)buf;
	f = strtod(s, &s);
	*કિંમત = f;
	પાછા (પૂર્ણાંક)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *કિંમત = f;
    પાછા (પૂર્ણાંક)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    સ્થિર પૂર્ણાંક
get_float_arg(typval_T *argvars, float_T *f)
{
    જો (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	પાછા OK;
    }
    જો (argvars[0].v_type == VAR_સંખ્યા)
    {
	*f = (float_T)argvars[0].vval.v_સંખ્યા;
	પાછા OK;
    }
    emsg(_(e_સંખ્યા_or_float_required));
    પાછા FAIL;
}

/*
 * "abs(expr)" function
 */
    શૂન્ય
f_abs(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    નહીંતર
    {
	varnumber_T	n;
	પૂર્ણાંક		error = FALSE;

	n = tv_get_સંખ્યા_chk(&argvars[0], &error);
	જો (error)
	    rettv->vval.v_સંખ્યા = -1;
	નહીંતર જો (n > 0)
	    rettv->vval.v_સંખ્યા = n;
	નહીંતર
	    rettv->vval.v_સંખ્યા = -n;
    }
}

/*
 * "acos()" function
 */
    શૂન્ય
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    શૂન્ય
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    શૂન્ય
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    શૂન્ય
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    જો (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    શૂન્ય
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    શૂન્ય
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    શૂન્ય
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    શૂન્ય
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    શૂન્ય
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    જો (get_float_arg(argvars, &f) != OK)
	પાછા;

    જો (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_સંખ્યા = -VARNUM_MAX;
    નહીંતર જો (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_સંખ્યા = VARNUM_MAX;
    નહીંતર
	rettv->vval.v_સંખ્યા = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    શૂન્ય
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    શૂન્ય
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    જો (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

#જો defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    શૂન્ય
f_isinf(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_સંખ્યા = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    શૂન્ય
f_isnan(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    શૂન્ય
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    શૂન્ય
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    શૂન્ય
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    જો (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    નહીંતર
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    પાછા f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    શૂન્ય
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    શૂન્ય
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    શૂન્ય
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    શૂન્ય
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    શૂન્ય
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    પૂર્ણાંક     isneg;
    પૂર્ણાંક	    skip_quotes;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    જો (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (શૂન્ય)string2float(p, &rettv->vval.v_float, skip_quotes);
    જો (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    શૂન્ય
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    શૂન્ય
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    શૂન્ય
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    જો (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->v_type = VAR_FLOAT;
    જો (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    નહીંતર
	rettv->vval.v_float = 0.0;
}

#endif
