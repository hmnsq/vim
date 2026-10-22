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
#تعریف_کرو USING_FLOAT_STUFF

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#اگر_تعریف_شدہ VMS
# include <float.h>
#اگر_ختم

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    صحیح
string2float(
    char_u	*متن,
    float_T	*قدر,	    // result stored here
    صحیح		skip_quotes)
{
    حرف	*s = (حرف *)متن;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    اگر (STRNICMP(متن, "inf", 3) == 0)
    {
	*قدر = INFINITY;
	واپس 3;
    }
    اگر (STRNICMP(متن, "-inf", 4) == 0)
    {
	*قدر = -INFINITY;
	واپس 4;
    }
    اگر (STRNICMP(متن, "nan", 3) == 0)
    {
	*قدر = NAN;
	واپس 3;
    }
    اگر (skip_quotes && vim_strchr((char_u *)s, '\'') != تہی)
    {
	char_u	    buf[100];
	char_u	    *p;
	صحیح	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	برائے (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    اگر (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    اگر (!vim_isdigit(*p))
		توڑو;
	}
	s = (حرف *)buf;
	f = strtod(s, &s);
	*قدر = f;
	واپس (صحیح)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *قدر = f;
    واپس (صحیح)((char_u *)s - متن);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    ساکن صحیح
get_float_arg(typval_T *argvars, float_T *f)
{
    اگر (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	واپس OK;
    }
    اگر (argvars[0].v_type == VAR_نمبر)
    {
	*f = (float_T)argvars[0].vval.v_نمبر;
	واپس OK;
    }
    emsg(_(e_نمبر_or_float_required));
    واپس FAIL;
}

/*
 * "abs(expr)" function
 */
    خالی
f_abs(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    ورنہ
    {
	varnumber_T	n;
	صحیح		error = FALSE;

	n = tv_get_نمبر_chk(&argvars[0], &error);
	اگر (error)
	    rettv->vval.v_نمبر = -1;
	ورنہ اگر (n > 0)
	    rettv->vval.v_نمبر = n;
	ورنہ
	    rettv->vval.v_نمبر = -n;
    }
}

/*
 * "acos()" function
 */
    خالی
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    خالی
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    خالی
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    خالی
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (اندر_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    خالی
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    خالی
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = جیب_تمام(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    خالی
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    خالی
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = قوت_نما(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    خالی
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    اگر (get_float_arg(argvars, &f) != OK)
	واپس;

    اگر (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_نمبر = -VARNUM_MAX;
    ورنہ اگر (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_نمبر = VARNUM_MAX;
    ورنہ
	rettv->vval.v_نمبر = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    خالی
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    خالی
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (اندر_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    ورنہ
	rettv->vval.v_float = 0.0;
}

#اگر defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    خالی
f_isinf(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_نمبر = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    خالی
f_isnan(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#اگر_ختم

/*
 * "log()" function
 */
    خالی
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = لوگارتھم(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    خالی
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    خالی
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    اگر (اندر_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = قوت(fx, fy);
    ورنہ
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_باری(float_T f)
{
    واپس f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    خالی
f_باری(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_باری(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    خالی
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = جیب(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    خالی
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    خالی
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = جذر(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    خالی
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    صحیح     isneg;
    صحیح	    skip_quotes;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    اگر (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (خالی)string2float(p, &rettv->vval.v_float, skip_quotes);
    اگر (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    خالی
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    خالی
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    خالی
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    اگر (اندر_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_FLOAT;
    اگر (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    ورنہ
	rettv->vval.v_float = 0.0;
}

#اگر_ختم
