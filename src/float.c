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

#include "vim.h"

#만약 defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    정수
string2float(
    char_u	*text,
    float_T	*value,	    // result stored here
    정수		skip_quotes)
{
    char	*s = (char *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    만약 (STRNICMP(text, "inf", 3) == 0)
    {
	*value = INFINITY;
	반환 3;
    }
    만약 (STRNICMP(text, "-inf", 4) == 0)
    {
	*value = -INFINITY;
	반환 4;
    }
    만약 (STRNICMP(text, "nan", 3) == 0)
    {
	*value = NAN;
	반환 3;
    }
    만약 (skip_quotes && vim_strchr((char_u *)s, '\'') != NULL)
    {
	char_u	    buf[100];
	char_u	    *p;
	정수	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	위해 (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    만약 (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    만약 (!vim_isdigit(*p))
		break;
	}
	s = (char *)buf;
	f = strtod(s, &s);
	*value = f;
	반환 (정수)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *value = f;
    반환 (정수)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    static 정수
get_float_arg(typval_T *argvars, float_T *f)
{
    만약 (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	반환 OK;
    }
    만약 (argvars[0].v_type == VAR_NUMBER)
    {
	*f = (float_T)argvars[0].vval.v_number;
	반환 OK;
    }
    emsg(_(e_number_or_float_required));
    반환 FAIL;
}

/*
 * "abs(expr)" function
 */
    void
f_abs(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    아니면
    {
	varnumber_T	n;
	정수		error = FALSE;

	n = tv_get_number_chk(&argvars[0], &error);
	만약 (error)
	    rettv->vval.v_number = -1;
	아니면 만약 (n > 0)
	    rettv->vval.v_number = n;
	아니면
	    rettv->vval.v_number = -n;
    }
}

/*
 * "acos()" function
 */
    void
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    void
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    void
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    void
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    만약 (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    void
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    void
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    void
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    void
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    void
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    만약 (get_float_arg(argvars, &f) != OK)
	반환;

    만약 (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_number = -VARNUM_MAX;
    아니면 만약 (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_number = VARNUM_MAX;
    아니면
	rettv->vval.v_number = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    void
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    void
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    만약 (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    아니면
	rettv->vval.v_float = 0.0;
}

#만약 defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    void
f_isinf(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_number = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    void
f_isnan(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->vval.v_number = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    void
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    void
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    void
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    만약 (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    아니면
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    반환 f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    void
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    void
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    void
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    void
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    void
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    정수     isneg;
    정수	    skip_quotes;

    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	반환;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    만약 (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (void)string2float(p, &rettv->vval.v_float, skip_quotes);
    만약 (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    void
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    void
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    아니면
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    void
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    만약 (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	반환;

    rettv->v_type = VAR_FLOAT;
    만약 (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    아니면
	rettv->vval.v_float = 0.0;
}

#endif
