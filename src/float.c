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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    ಪೂರ್ಣಾಙ್ಕ
string2float(
    char_u	*text,
    float_T	*ಮೌಲ್ಯ,	    // result stored here
    ಪೂರ್ಣಾಙ್ಕ		skip_quotes)
{
    ವರ್ಣ	*s = (ವರ್ಣ *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    ಯದಿ (STRNICMP(text, "inf", 3) == 0)
    {
	*ಮೌಲ್ಯ = INFINITY;
	ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ (STRNICMP(text, "-inf", 4) == 0)
    {
	*ಮೌಲ್ಯ = -INFINITY;
	ಪ್ರತ್ಯಾಗಮ 4;
    }
    ಯದಿ (STRNICMP(text, "nan", 3) == 0)
    {
	*ಮೌಲ್ಯ = NAN;
	ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ (skip_quotes && vim_strchr((char_u *)s, '\'') != ರಿಕ್ತ)
    {
	char_u	    buf[100];
	char_u	    *p;
	ಪೂರ್ಣಾಙ್ಕ	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	ಪ್ರತಿ (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    ಯದಿ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    ಯದಿ (!vim_isdigit(*p))
		ಭಙ್ಗ;
	}
	s = (ವರ್ಣ *)buf;
	f = strtod(s, &s);
	*ಮೌಲ್ಯ = f;
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *ಮೌಲ್ಯ = f;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_float_arg(typval_T *argvars, float_T *f)
{
    ಯದಿ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ)
    {
	*f = (float_T)argvars[0].vval.v_ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    emsg(_(e_ಸಂಖ್ಯೆ_or_float_required));
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * "abs(expr)" function
 */
    ಶೂನ್ಯ
f_abs(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    ಅನ್ಯಥಾ
    {
	varnumber_T	n;
	ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	ಯದಿ (error)
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಅನ್ಯಥಾ ಯದಿ (n > 0)
	    rettv->vval.v_ಸಂಖ್ಯೆ = n;
	ಅನ್ಯಥಾ
	    rettv->vval.v_ಸಂಖ್ಯೆ = -n;
    }
}

/*
 * "acos()" function
 */
    ಶೂನ್ಯ
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    ಶೂನ್ಯ
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    ಶೂನ್ಯ
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    ಶೂನ್ಯ
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಯದಿ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    ಶೂನ್ಯ
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    ಶೂನ್ಯ
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    ಶೂನ್ಯ
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    ಶೂನ್ಯ
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    ಶೂನ್ಯ
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (get_float_arg(argvars, &f) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_ಸಂಖ್ಯೆ = -VARNUM_MAX;
    ಅನ್ಯಥಾ ಯದಿ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_ಸಂಖ್ಯೆ = VARNUM_MAX;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    ಶೂನ್ಯ
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    ಶೂನ್ಯ
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಯದಿ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

#ಯದಿ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    ಶೂನ್ಯ
f_isinf(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_ಸಂಖ್ಯೆ = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    ಶೂನ್ಯ
f_isnan(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    ಶೂನ್ಯ
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    ಶೂನ್ಯ
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    ಶೂನ್ಯ
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಯದಿ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    ಪ್ರತ್ಯಾಗಮ f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    ಶೂನ್ಯ
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    ಶೂನ್ಯ
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    ಶೂನ್ಯ
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    ಶೂನ್ಯ
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    ಶೂನ್ಯ
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    ಪೂರ್ಣಾಙ್ಕ     isneg;
    ಪೂರ್ಣಾಙ್ಕ	    skip_quotes;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    ಯದಿ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (ಶೂನ್ಯ)string2float(p, &rettv->vval.v_float, skip_quotes);
    ಯದಿ (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    ಶೂನ್ಯ
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    ಶೂನ್ಯ
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    ಶೂನ್ಯ
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಯದಿ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_FLOAT;
    ಯದಿ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    ಅನ್ಯಥಾ
	rettv->vval.v_float = 0.0;
}

#endif
