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

#यदि defined(FEAT_EVAL)

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
    float_T	*मान,	    // result stored here
    पूर्णाङ्क		skip_quotes)
{
    वर्ण	*s = (वर्ण *)text;
    float_T	f;

    // MS-Windows does not deal with "inf" and "nan" properly.
    यदि (STRNICMP(text, "inf", 3) == 0)
    {
	*मान = अनन्त;
	फर्काऊ 3;
    }
    यदि (STRNICMP(text, "-inf", 4) == 0)
    {
	*मान = -अनन्त;
	फर्काऊ 4;
    }
    यदि (STRNICMP(text, "nan", 3) == 0)
    {
	*मान = न_अंक;
	फर्काऊ 3;
    }
    यदि (skip_quotes && vim_strchr((char_u *)s, '\'') != रिक्त)
    {
	char_u	    buf[100];
	char_u	    *p;
	पूर्णाङ्क	    quotes = 0;

	vim_strncpy(buf, (char_u *)s, 99);
	लागि (p = buf; ; p = skipdigits(p))
	{
	    // remove single quotes between digits, not in the exponent
	    यदि (*p == '\'')
	    {
		++quotes;
		mch_memmove(p, p + 1, STRLEN(p));
	    }
	    यदि (!vim_isdigit(*p))
		रोक;
	}
	s = (वर्ण *)buf;
	f = म_से_भग्न(s, &s);
	*मान = f;
	फर्काऊ (पूर्णाङ्क)((char_u *)s - buf) + quotes;
    }

    f = म_से_भग्न(s, &s);
    *मान = f;
    फर्काऊ (पूर्णाङ्क)((char_u *)s - text);
}

/*
 * Get the float value of "argvars[0]" into "f".
 * Returns FAIL when the argument is not a Number or Float.
 */
    स्थिर पूर्णाङ्क
get_float_arg(typval_T *argvars, float_T *f)
{
    यदि (argvars[0].v_type == VAR_FLOAT)
    {
	*f = argvars[0].vval.v_float;
	फर्काऊ OK;
    }
    यदि (argvars[0].v_type == VAR_सङ्ख्या)
    {
	*f = (float_T)argvars[0].vval.v_सङ्ख्या;
	फर्काऊ OK;
    }
    emsg(_(e_सङ्ख्या_or_float_required));
    फर्काऊ FAIL;
}

/*
 * "abs(expr)" function
 */
    शून्य
f_abs(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (argvars[0].v_type == VAR_FLOAT)
    {
	rettv->v_type = VAR_FLOAT;
	rettv->vval.v_float = भ_असल(argvars[0].vval.v_float);
    }
    अन्यथा
    {
	varnumber_T	n;
	पूर्णाङ्क		error = FALSE;

	n = tv_get_सङ्ख्या_chk(&argvars[0], &error);
	यदि (error)
	    rettv->vval.v_सङ्ख्या = -1;
	अन्यथा यदि (n > 0)
	    rettv->vval.v_सङ्ख्या = n;
	अन्यथा
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

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = acos(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "asin()" function
 */
    शून्य
f_asin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = asin(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "atan()" function
 */
    शून्य
f_atan(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = atan(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "atan2()" function
 */
    शून्य
f_atan2(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    यदि (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = atan2(fx, fy);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "ceil({float})" function
 */
    शून्य
f_ceil(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = उच्चमान(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "cos()" function
 */
    शून्य
f_cos(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cos(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "cosh()" function
 */
    शून्य
f_cosh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = cosh(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "exp()" function
 */
    शून्य
f_exp(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = exp(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "float2nr({float})" function
 */
    शून्य
f_float2nr(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (get_float_arg(argvars, &f) != OK)
	फर्काऊ;

    यदि (f <= (float_T)-VARNUM_MAX + द्विग_अंतर)
	rettv->vval.v_सङ्ख्या = -VARNUM_MAX;
    अन्यथा यदि (f >= (float_T)VARNUM_MAX - द्विग_अंतर)
	rettv->vval.v_सङ्ख्या = VARNUM_MAX;
    अन्यथा
	rettv->vval.v_सङ्ख्या = (varnumber_T)f;
}

/*
 * "floor({float})" function
 */
    शून्य
f_floor(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = न्यूनमान(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "fmod()" function
 */
    शून्य
f_fmod(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    यदि (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = भ_शेष(fx, fy);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

#यदि defined(HAVE_MATH_H)
/*
 * "isinf()" function
 */
    शून्य
f_isinf(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (argvars[0].v_type == VAR_FLOAT && isinf(argvars[0].vval.v_float))
	rettv->vval.v_सङ्ख्या = argvars[0].vval.v_float > 0.0 ? 1 : -1;
}

/*
 * "isnan()" function
 */
    शून्य
f_isnan(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

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

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "log10()" function
 */
    शून्य
f_log10(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = log10(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "pow()" function
 */
    शून्य
f_pow(typval_T *argvars, typval_T *rettv)
{
    float_T	fx = 0.0, fy = 0.0;

    यदि (in_vim9script()
	    && (check_for_float_or_nr_arg(argvars, 0) == FAIL
		|| check_for_float_or_nr_arg(argvars, 1) == FAIL))
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &fx) == OK
				     && get_float_arg(&argvars[1], &fy) == OK)
	rettv->vval.v_float = घात(fx, fy);
    अन्यथा
	rettv->vval.v_float = 0.0;
}


/*
 * round() is not in C90, use ceil() or floor() instead.
 */
    float_T
vim_round(float_T f)
{
    फर्काऊ f > 0 ? न्यूनमान(f + 0.5) : उच्चमान(f - 0.5);
}

/*
 * "round({float})" function
 */
    शून्य
f_round(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = vim_round(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "sin()" function
 */
    शून्य
f_sin(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sin(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "sinh()" function
 */
    शून्य
f_sinh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = sinh(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "sqrt()" function
 */
    शून्य
f_sqrt(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = वर्ग_मूल(f);
    अन्यथा
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

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    skip_quotes = argvars[1].v_type != VAR_UNKNOWN && tv_get_bool(&argvars[1]);

    p = skipwhite(tv_get_string_strict(&argvars[0]));
    isneg = (*p == '-');

    यदि (*p == '+' || *p == '-')
	p = skipwhite(p + 1);
    (शून्य)string2float(p, &rettv->vval.v_float, skip_quotes);
    यदि (isneg)
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

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tan(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "tanh()" function
 */
    शून्य
f_tanh(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	rettv->vval.v_float = tanh(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

/*
 * "trunc({float})" function
 */
    शून्य
f_trunc(typval_T *argvars, typval_T *rettv)
{
    float_T	f = 0.0;

    यदि (in_vim9script() && check_for_float_or_nr_arg(argvars, 0) == FAIL)
	फर्काऊ;

    rettv->v_type = VAR_FLOAT;
    यदि (get_float_arg(argvars, &f) == OK)
	// trunc() is not in C90, use floor() or ceil() instead.
	rettv->vval.v_float = f > 0 ? न्यूनमान(f) : उच्चमान(f);
    अन्यथा
	rettv->vval.v_float = 0.0;
}

#endif
