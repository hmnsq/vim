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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# include <float.h>
#ਜੇ_ਅੰਤ

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    ਪੂਰਨ
string2float(
    char_u	*ਪਾਠ,
    float_T	*ਮੁੱਲ,	    // result stored here
    ਪੂਰਨ		skip_quotes)
{
    ਅੱਖਰ	*s = (ਅੱਖਰ *)ਪਾਠ;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    ਜੇ (STRNICMP(ਪਾਠ, "inf", 3) == 0)
    {
	*ਮੁੱਲ = INFINITY;
	ਵਾਪਸ 3;
    }
    ਜੇ (STRNICMP(ਪਾਠ, "-inf", 4) == 0)
    {
	*ਮੁੱਲ = -INFINITY;
	ਵਾਪਸ 4;
    }
    ਜੇ (STRNICMP(ਪਾਠ, "nan", 3) == 0)
    {
	*ਮੁੱਲ = NAN;
	ਵਾਪਸ 3;
    }
    ਜੇ (skip_quotes && vim_strchr((char_u *)s, '\'') != ਨਿਰਾ)
    {
	char_u	    buf[100];
	char_u	    *p;
	ਪੂਰਨ	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	ਲਈ (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    ਜੇ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    ਜੇ (!vim_isdigit(*p))
		ਰੋਕੋ;
	}
	s = (ਅੱਖਰ *)buf;
	f = strtod(s, &s);
	*ਮੁੱਲ = f;
	ਵਾਪਸ (ਪੂਰਨ)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *ਮੁੱਲ = f;
    ਵਾਪਸ (ਪੂਰਨ)((char_u *)s - ਪਾਠ);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ਸਥਿਰ ਪੂਰਨ
get_float_arg(typval_T *argvars, float_T *f)
{
    ਜੇ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	ਵਾਪਸ OK;
    }
    ਜੇ (argvars[0].v_type == VAR_ਸੰਖਿਆ)
    {
	*f = (float_T)argvars[0].vval.v_ਸੰਖਿਆ;
	ਵਾਪਸ OK;
    }
    emsg(_(e_ਸੰਖਿਆ_or_float_required));
    ਵਾਪਸ FAIL;
}

/*
 * "abs(expr)" function
 */
    ਖਾਲੀ
f_abs(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    ਨਹੀਂਤਾਂ
    {
	varnumber_T	n;
	ਪੂਰਨ		error = FALSE;

	n = tv_get_ਸੰਖਿਆ_chk(&argvars[0], &error);
	ਜੇ (error)
	    rettv->vval.v_ਸੰਖਿਆ = -1;
	ਨਹੀਂਤਾਂ ਜੇ (n > 0)
	    rettv->vval.v_ਸੰਖਿਆ = n;
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_ਸੰਖਿਆ = -n;
    }
}

/*
 * "acos()" function
 */
    ਖਾਲੀ
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    ਖਾਲੀ
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    ਖਾਲੀ
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    ਖਾਲੀ
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    ਖਾਲੀ
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    ਖਾਲੀ
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ਕੋਜੀਵਾ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    ਖਾਲੀ
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    ਖਾਲੀ
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ਘਾਤਾਂਕ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    ਖਾਲੀ
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (get_float_arg(argvars, &f) != OK)
	ਵਾਪਸ;

    ਜੇ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_ਸੰਖਿਆ = -VARNUM_MAX;
    ਨਹੀਂਤਾਂ ਜੇ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_ਸੰਖਿਆ = VARNUM_MAX;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    ਖਾਲੀ
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    ਖਾਲੀ
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

#ਜੇ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    ਖਾਲੀ
f_isinf(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_ਸੰਖਿਆ = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    ਖਾਲੀ
f_isnan(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#ਜੇ_ਅੰਤ

/*
 * "log()" function
 */
    ਖਾਲੀ
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ਲਘੂਗਣਕ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    ਖਾਲੀ
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    ਖਾਲੀ
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = ਘਾਤ(fx, fy);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_ਗੇੜਾ(float_T f)
{
    ਵਾਪਸ f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    ਖਾਲੀ
f_ਗੇੜਾ(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_ਗੇੜਾ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    ਖਾਲੀ
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ਜੀਵਾ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    ਖਾਲੀ
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    ਖਾਲੀ
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ਵਰਗਮੂਲ(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    ਖਾਲੀ
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    ਪੂਰਨ     isneg;
    ਪੂਰਨ	    skip_quotes;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    ਜੇ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (ਖਾਲੀ)string2float(p, &rettv->vval.v_float, skip_quotes);
    ਜੇ (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    ਖਾਲੀ
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    ਖਾਲੀ
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    ਖਾਲੀ
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_FLOAT;
    ਜੇ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_float = 0.0;
}

#ਜੇ_ਅੰਤ
