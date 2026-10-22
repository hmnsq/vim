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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    صحيح
string2float(
    char_u	*text,
    float_T	*نيلاي,	    // result stored here
    صحيح		skip_quotes)
{
    حرف	*s = (حرف *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    إذا (STRNICMP(text, "inf", 3) == 0)
    {
	*نيلاي = INFINITY;
	أرجع 3;
    }
    إذا (STRNICMP(text, "-inf", 4) == 0)
    {
	*نيلاي = -INFINITY;
	أرجع 4;
    }
    إذا (STRNICMP(text, "nan", 3) == 0)
    {
	*نيلاي = NAN;
	أرجع 3;
    }
    إذا (skip_quotes && vim_strchr((char_u *)s, '\'') != عدم)
    {
	char_u	    buf[100];
	char_u	    *p;
	صحيح	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	لكل (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    إذا (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    إذا (!vim_isdigit(*p))
		اكسر;
	}
	s = (حرف *)buf;
	f = strtod(s, &s);
	*نيلاي = f;
	أرجع (صحيح)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *نيلاي = f;
    أرجع (صحيح)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ساكن صحيح
get_float_arg(typval_T *argvars, float_T *f)
{
    إذا (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	أرجع OK;
    }
    إذا (argvars[0].v_type == VAR_نومبور)
    {
	*f = (float_T)argvars[0].vval.v_نومبور;
	أرجع OK;
    }
    emsg(_(e_نومبور_or_float_required));
    أرجع FAIL;
}

/*
 * "abs(expr)" function
 */
    فراغ
f_abs(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    وإلا
    {
	varnumber_T	n;
	صحيح		error = FALSE;

	n = tv_get_نومبور_chk(&argvars[0], &error);
	إذا (error)
	    rettv->vval.v_نومبور = -1;
	وإلا إذا (n > 0)
	    rettv->vval.v_نومبور = n;
	وإلا
	    rettv->vval.v_نومبور = -n;
    }
}

/*
 * "acos()" function
 */
    فراغ
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    فراغ
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    فراغ
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    فراغ
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    إذا (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    فراغ
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    فراغ
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    فراغ
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    فراغ
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    فراغ
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (get_float_arg(argvars, &f) != OK)
	أرجع;

    إذا (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_نومبور = -VARNUM_MAX;
    وإلا إذا (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_نومبور = VARNUM_MAX;
    وإلا
	rettv->vval.v_نومبور = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    فراغ
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    فراغ
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    إذا (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    وإلا
	rettv->vval.v_float = 0.0;
}

#إذا defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    فراغ
f_isinf(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_نومبور = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    فراغ
f_isnan(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_نومبور = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    فراغ
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    فراغ
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    فراغ
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    إذا (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    وإلا
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    أرجع f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    فراغ
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    فراغ
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    فراغ
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    فراغ
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    فراغ
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    صحيح     isneg;
    صحيح	    skip_quotes;

    إذا (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    إذا (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (فراغ)string2float(p, &rettv->vval.v_float, skip_quotes);
    إذا (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    فراغ
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    فراغ
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    فراغ
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    إذا (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_FLOAT;
    إذا (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    وإلا
	rettv->vval.v_float = 0.0;
}

#endif
