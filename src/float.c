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
#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <float.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    ಪೂರ್ಣಾಂಕ
string2float(
    char_u	*ಪಠ್ಯ,
    float_T	*ಮೌಲ್ಯ,	    // result stored here
    ಪೂರ್ಣಾಂಕ		skip_quotes)
{
    ಅಕ್ಷರ	*s = (ಅಕ್ಷರ *)ಪಠ್ಯ;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    ಆದರೆ (STRNICMP(ಪಠ್ಯ, "inf", 3) == 0)
    {
	*ಮೌಲ್ಯ = INFINITY;
	ಹಿಂತಿರುಗಿ 3;
    }
    ಆದರೆ (STRNICMP(ಪಠ್ಯ, "-inf", 4) == 0)
    {
	*ಮೌಲ್ಯ = -INFINITY;
	ಹಿಂತಿರುಗಿ 4;
    }
    ಆದರೆ (STRNICMP(ಪಠ್ಯ, "nan", 3) == 0)
    {
	*ಮೌಲ್ಯ = NAN;
	ಹಿಂತಿರುಗಿ 3;
    }
    ಆದರೆ (skip_quotes && vim_strchr((char_u *)s, '\'') != ಖಾಲಿ)
    {
	char_u	    buf[100];
	char_u	    *p;
	ಪೂರ್ಣಾಂಕ	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	ಗಾಗಿ (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    ಆದರೆ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    ಆದರೆ (!vim_isdigit(*p))
		ನಿಲ್ಲಿಸು;
	}
	s = (ಅಕ್ಷರ *)buf;
	f = strtod(s, &s);
	*ಮೌಲ್ಯ = f;
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *ಮೌಲ್ಯ = f;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)((char_u *)s - ಪಠ್ಯ);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_float_arg(typval_T *argvars, float_T *f)
{
    ಆದರೆ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ)
    {
	*f = (float_T)argvars[0].vval.v_ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ OK;
    }
    emsg(_(e_ಸಂಖ್ಯೆ_or_float_required));
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * "abs(expr)" function
 */
    ಶೂನ್ಯ
f_abs(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	varnumber_T	n;
	ಪೂರ್ಣಾಂಕ		error = FALSE;

	n = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], &error);
	ಆದರೆ (error)
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n > 0)
	    rettv->vval.v_ಸಂಖ್ಯೆ = n;
	ಇಲ್ಲದಿದ್ದರೆ
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

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    ಶೂನ್ಯ
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    ಶೂನ್ಯ
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    ಶೂನ್ಯ
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    ಶೂನ್ಯ
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    ಶೂನ್ಯ
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ಕೋಜ್ಯಾ(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    ಶೂನ್ಯ
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    ಶೂನ್ಯ
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ಘಾತಾಂಕ(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    ಶೂನ್ಯ
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (get_float_arg(argvars, &f) != OK)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_ಸಂಖ್ಯೆ = -VARNUM_MAX;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_ಸಂಖ್ಯೆ = VARNUM_MAX;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    ಶೂನ್ಯ
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    ಶೂನ್ಯ
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

#ಆದರೆ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    ಶೂನ್ಯ
f_isinf(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_ಸಂಖ್ಯೆ = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    ಶೂನ್ಯ
f_isnan(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "log()" function
 */
    ಶೂನ್ಯ
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ಲಘುಗಣಕ(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    ಶೂನ್ಯ
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    ಶೂನ್ಯ
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = ಘಾತ(fx, fy);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_ಸುತ್ತು(float_T f)
{
    ಹಿಂತಿರುಗಿ f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    ಶೂನ್ಯ
f_ಸುತ್ತು(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_ಸುತ್ತು(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    ಶೂನ್ಯ
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ಜ್ಯಾ(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    ಶೂನ್ಯ
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    ಶೂನ್ಯ
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ವರ್ಗಮೂಲ(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    ಶೂನ್ಯ
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    ಪೂರ್ಣಾಂಕ     isneg;
    ಪೂರ್ಣಾಂಕ	    skip_quotes;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    ಆದರೆ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (ಶೂನ್ಯ)string2float(p, &rettv->vval.v_float, skip_quotes);
    ಆದರೆ (isneg)
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

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    ಶೂನ್ಯ
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    ಶೂನ್ಯ
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    ಆದರೆ (ಒಳ_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_FLOAT;
    ಆದರೆ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_float = 0.0;
}

#ಆದರೆ_ಅಂತ್ಯ
