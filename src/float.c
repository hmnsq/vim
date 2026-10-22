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

#གལ་སྲིད defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    ཧྲིལ
string2float(
    char_u	*text,
    float_T	*value,	    // result stored here
    ཧྲིལ		skip_quotes)
{
    var_na	*s = (var_na *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    གལ་སྲིད (STRNICMP(text, "inf", 3) == 0)
    {
	*value = INFINITY;
	ལོག 3;
    }
    གལ་སྲིད (STRNICMP(text, "-inf", 4) == 0)
    {
	*value = -INFINITY;
	ལོག 4;
    }
    གལ་སྲིད (STRNICMP(text, "nan", 3) == 0)
    {
	*value = NAN;
	ལོག 3;
    }
    གལ་སྲིད (skip_quotes && vim_strchr((char_u *)s, '\'') != NULL)
    {
	char_u	    buf[100];
	char_u	    *p;
	ཧྲིལ	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	དོན་ལུ (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    གལ་སྲིད (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    གལ་སྲིད (!vim_isdigit(*p))
		bha_nga;
	}
	s = (var_na *)buf;
	f = strtod(s, &s);
	*value = f;
	ལོག (ཧྲིལ)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *value = f;
    ལོག (ཧྲིལ)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    static ཧྲིལ
get_float_arg(typval_T *argvars, float_T *f)
{
    གལ་སྲིད (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	ལོག OK;
    }
    གལ་སྲིད (argvars[0].v_type == VAR_NUMBER)
    {
	*f = (float_T)argvars[0].vval.v_number;
	ལོག OK;
    }
    emsg(_(e_number_or_float_required));
    ལོག FAIL;
}

/*
 * "abs(expr)" function
 */
    void
f_abs(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    གལ་སྲིད (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    དེ་མིན
    {
	varnumber_T	n;
	ཧྲིལ		error = FALSE;

	n = tv_get_number_chk(&argvars[0], &error);
	གལ་སྲིད (error)
	    rettv->vval.v_number = -1;
	དེ་མིན གལ་སྲིད (n > 0)
	    rettv->vval.v_number = n;
	དེ་མིན
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

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    void
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    void
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    void
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    གལ་སྲིད (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    void
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    void
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    void
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    void
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    void
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    གལ་སྲིད (get_float_arg(argvars, &f) != OK)
	ལོག;

    གལ་སྲིད (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_number = -VARNUM_MAX;
    དེ་མིན གལ་སྲིད (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_number = VARNUM_MAX;
    དེ་མིན
	rettv->vval.v_number = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    void
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    void
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    གལ་སྲིད (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

#གལ་སྲིད defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    void
f_isinf(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    གལ་སྲིད (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_number = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    void
f_isnan(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

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

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    void
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    void
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    གལ་སྲིད (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    ལོག f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    void
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    void
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    void
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    void
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    void
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    ཧྲིལ     isneg;
    ཧྲིལ	    skip_quotes;

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    གལ་སྲིད (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (void)string2float(p, &rettv->vval.v_float, skip_quotes);
    གལ་སྲིད (isneg)
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

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    void
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    void
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    གལ་སྲིད (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->v_type = VAR_FLOAT;
    གལ་སྲིད (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    དེ་མིན
	rettv->vval.v_float = 0.0;
}

#endif
