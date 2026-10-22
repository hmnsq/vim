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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

/*
 * Convert the string "text" to a floating point number.
 * This uses strtod().  setlocale(LC_NUMERIC, "C") has been used to make sure
 * this always uses a decimal point.
 * Returns the length of the text that was consumed.
 */
    पूर्णाङ्क
string2float(
    char_u	*text,
    float_T	*मानम्,	    // result stored here
    पूर्णाङ्क		skip_quotes)
{
    वर्ण	*s = (वर्ण *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    जइ (STRNICMP(text, "inf", 3) == 0)
    {
	*मानम् = INFINITY;
	प्रत्यागम 3;
    }
    जइ (STRNICMP(text, "-inf", 4) == 0)
    {
	*मानम् = -INFINITY;
	प्रत्यागम 4;
    }
    जइ (STRNICMP(text, "nan", 3) == 0)
    {
	*मानम् = NAN;
	प्रत्यागम 3;
    }
    जइ (skip_quotes && vim_strchr((char_u *)s, '\'') != रिक्त)
    {
	char_u	    buf[100];
	char_u	    *p;
	पूर्णाङ्क	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	प्रति (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    जइ (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    जइ (!vim_isdigit(*p))
		भङ्ग;
	}
	s = (वर्ण *)buf;
	f = strtod(s, &s);
	*मानम् = f;
	प्रत्यागम (पूर्णाङ्क)((char_u *)s - buf) + quotes;
    }

    f = strtod(s, &s);
    *मानम् = f;
    प्रत्यागम (पूर्णाङ्क)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    स्थिर पूर्णाङ्क
get_float_arg(typval_T *argvars, float_T *f)
{
    जइ (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	प्रत्यागम OK;
    }
    जइ (argvars[0].v_type == VAR_सङ्ख्या)
    {
	*f = (float_T)argvars[0].vval.v_सङ्ख्या;
	प्रत्यागम OK;
    }
    emsg(_(e_सङ्ख्या_or_float_required));
    प्रत्यागम FAIL;
}

/*
 * "abs(expr)" function
 */
    शून्य
f_abs(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = fabs(argvars[0].vval.v_float);
    }
    अण्णहा
    {
	varnumber_T	n;
	पूर्णाङ्क		error = FALSE;

	n = tv_get_सङ्ख्या_chk(&argvars[0], &error);
	जइ (error)
	    rettv->vval.v_सङ्ख्या = -1;
	अण्णहा जइ (n > 0)
	    rettv->vval.v_सङ्ख्या = n;
	अण्णहा
	    rettv->vval.v_सङ्ख्या = -n;
    }
}

/*
 * "acos()" function
 */
    शून्य
f_acos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    शून्य
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    शून्य
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    शून्य
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    जइ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    शून्य
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = ceil(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    शून्य
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    शून्य
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    शून्य
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    शून्य
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (get_float_arg(argvars, &f) != OK)
	प्रत्यागम;

    जइ (f <= (float_T)-VARNUM_MAX + DBL_EPSILON)
	rettv->vval.v_सङ्ख्या = -VARNUM_MAX;
    अण्णहा जइ (f >= (float_T)VARNUM_MAX - DBL_EPSILON)
	rettv->vval.v_सङ्ख्या = VARNUM_MAX;
    अण्णहा
	rettv->vval.v_सङ्ख्या = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    शून्य
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = floor(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    शून्य
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    जइ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = fmod(fx, fy);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

#जइ defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    शून्य
f_isinf(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_सङ्ख्या = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    शून्य
f_isnan(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = argvars[0].v_type == VAR_FLOAT
					    && isnan(argvars[0].vval.v_float);
}
#endif

/*
 * "log()" function
 */
    शून्य
f_log(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    शून्य
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    शून्य
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    जइ (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = pow(fx, fy);
    अण्णहा
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    प्रत्यागम f > 0 ? floor(f + 0.5) : ceil(f - 0.5);
}

/*
 * "round({float})" function
 */
    शून्य
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    शून्य
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    शून्य
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    शून्य
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sqrt(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "str2float()" function
 */
    शून्य
f_str2float(typval_T *argvars, typval_T *rettv)
{
    char_u *p;
    पूर्णाङ्क     isneg;
    पूर्णाङ्क	    skip_quotes;

    जइ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    जइ (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (शून्य)string2float(p, &rettv->vval.v_float, skip_quotes);
    जइ (isneg)
	rettv->vval.v_float *= -1;
    rettv->v_type = VAR_FLOAT;
}

/*
 * "tan()" function
 */
    शून्य
f_tan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    शून्य
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    शून्य
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    जइ (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->v_type = VAR_FLOAT;
    जइ (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? floor(f) : ceil(f);
    अण्णहा
	rettv->vval.v_float = 0.0;
}

#endif
