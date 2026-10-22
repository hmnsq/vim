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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    പൂർണ്ണസംഖ്യ
string2float(
    char_u	*text,
    float_T	*മൂല്യം,	    // result stored here
    പൂർണ്ണസംഖ്യ		skip_quotes)
{
    അക്ഷരം	*s = (അക്ഷരം *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    എങ്കിൽ (STRNICMP(text, "inf", 3) == 0)
    {
	*മൂല്യം = INFINITY;
	തിരികെ 3;
    }
    എങ്കിൽ (STRNICMP(text, "-inf", 4) == 0)
    {
	*മൂല്യം = -INFINITY;
	തിരികെ 4;
    }
    എങ്കിൽ (STRNICMP(text, "nan", 3) == 0)
    {
	*മൂല്യം = NAN;
	തിരികെ 3;
    }
    എങ്കിൽ (skip_quotes && vim_strchr((char_u *)s, '\'') != ശൂന്യ)
    {
	char_u	    buf[100];
	char_u	    *p;
	പൂർണ്ണസംഖ്യ	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	വേണ്ടി (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    എങ്കിൽ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    എങ്കിൽ (!vim_isdigit(*p))
		നിർത്തുക;
	}
	s = (അക്ഷരം *)buf;
	f = strtod(s, &s);
	*മൂല്യം = f;
	തിരികെ (പൂർണ്ണസംഖ്യ)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *മൂല്യം = f;
    തിരികെ (പൂർണ്ണസംഖ്യ)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_float_arg(typval_T *argvars, float_T *f)
{
    എങ്കിൽ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	തിരികെ OK;
    }
    എങ്കിൽ (argvars[0].v_type == VAR_സംഖ്യ)
    {
	*f = (float_T)argvars[0].vval.v_സംഖ്യ;
	തിരികെ OK;
    }
    emsg(_(e_സംഖ്യ_or_float_required));
    തിരികെ FAIL;
}

/*
 * "abs(expr)" function
 */
    ശൂന്യം
f_abs(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    അല്ലെങ്കിൽ
    {
	varnumber_T	n;
	പൂർണ്ണസംഖ്യ		error = FALSE;

	n = tv_get_സംഖ്യ_chk(&argvars[0], &error);
	എങ്കിൽ (error)
	    rettv->vval.v_സംഖ്യ = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (n > 0)
	    rettv->vval.v_സംഖ്യ = n;
	അല്ലെങ്കിൽ
	    rettv->vval.v_സംഖ്യ = -n;
    }
}

/*
 * "acos()" function
 */
    ശൂന്യം
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    ശൂന്യം
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    ശൂന്യം
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    ശൂന്യം
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    ശൂന്യം
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    ശൂന്യം
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    ശൂന്യം
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    ശൂന്യം
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    ശൂന്യം
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (get_float_arg(argvars, &f) != OK)
	തിരികെ;

    എങ്കിൽ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_സംഖ്യ = -VARNUM_MAX;
    അല്ലെങ്കിൽ എങ്കിൽ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_സംഖ്യ = VARNUM_MAX;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    ശൂന്യം
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    ശൂന്യം
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

#എങ്കിൽ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    ശൂന്യം
f_isinf(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_സംഖ്യ = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    ശൂന്യം
f_isnan(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    ശൂന്യം
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    ശൂന്യം
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    ശൂന്യം
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    എങ്കിൽ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    തിരികെ f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    ശൂന്യം
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    ശൂന്യം
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    ശൂന്യം
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    ശൂന്യം
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    ശൂന്യം
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    പൂർണ്ണസംഖ്യ     isneg;
    പൂർണ്ണസംഖ്യ	    skip_quotes;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    എങ്കിൽ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (ശൂന്യം)string2float(p, &rettv->vval.v_float, skip_quotes);
    എങ്കിൽ (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    ശൂന്യം
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    ശൂന്യം
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    ശൂന്യം
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    എങ്കിൽ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_FLOAT;
    എങ്കിൽ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    അല്ലെങ്കിൽ
	rettv->vval.v_float = 0.0;
}

#endif
