/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * strptime.c: portable strptime() fallback for platforms whose C runtime
 * does not provide one (currently the MSVC / MinGW CRT on Windows).
 *
 * Ported from NetBSD's lib/libc/time/strptime.c (rev 1.67, 2024-06-07).
 * The original BSD 2-clause licence follows below and must be preserved.
 * Windows-specific adjustments:
 *   - Locale-specific weekday / month / AM-PM tables are hard-coded in
 *     English; this matches what strftime() emits in the "C" locale on
 *     Windows and is sufficient for Vim's strptime() test suite.
 *   - The NetBSD-specific fromzone()/tzalloc() path is stubbed out because
 *     the Windows CRT does not ship the IANA tzfile loader.
 *   - The tm_gmtoff / tm_zone BSD extensions are not available in the
 *     Windows CRT's struct tm, so the %Z / %z conversion parses the input
 *     without storing the offset (matching NetBSD behavior when those
 *     fields are not compiled in).
 */

/*-
 * Copyright (c) 1997, 1998, 2005, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 * Heavily optimised by David Laight
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#شامل "vim.h"

#اگر_تعریف_شدہ MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#تعریف_کرو TM_YEAR_بنیاد	1900
#تعریف_کرو TM_SUNDAY	0
#تعریف_کرو TM_MONDAY	1
#تعریف_کرو SECSPERHOUR	3600
#تعریف_کرو SECSPERMIN	60
#تعریف_کرو HOURSPERDAY	24

#تعریف_کرو isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#تعریف_کرو isleap_مجموع(a, b) \
			isleap(((غیرعلامتی)(a) + (غیرعلامتی)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#اگر defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#اگر_ختم

/* Locale tables (English / "C" locale). */
ساکن ثابت حرف *ثابت c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ساکن ثابت حرف *ثابت c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ساکن ثابت حرف *ثابت c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ساکن ثابت حرف *ثابت c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ساکن ثابت حرف *ثابت c_am_pm[] = { "AM", "PM" };
ساکن ثابت حرف c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ساکن ثابت حرف c_d_fmt[] = "%m/%d/%y";
ساکن ثابت حرف c_t_fmt[] = "%H:%M:%S";
ساکن ثابت حرف c_t_fmt_ampm[] = "%I:%M:%S %p";

ساکن ثابت غیرعلامتی حرف *conv_عدد(ثابت غیرعلامتی حرف *, صحیح *,
						غیرعلامتی صحیح, غیرعلامتی صحیح);
ساکن ثابت غیرعلامتی حرف *ڈھونڈو_string(ثابت غیرعلامتی حرف *, صحیح *,
			ثابت حرف * ثابت *, ثابت حرف * ثابت *, صحیح);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#تعریف_کرو ALT_E			0x01
#تعریف_کرو ALT_O			0x02
#تعریف_کرو LEGAL_ALT(x)		{ اگر (alt_format & ~(x)) واپس تہی; }

#تعریف_کرو S_YEAR			(1 << 0)
#تعریف_کرو S_MON			(1 << 1)
#تعریف_کرو S_YDAY			(1 << 2)
#تعریف_کرو S_MDAY			(1 << 3)
#تعریف_کرو S_WDAY			(1 << 4)
#تعریف_کرو S_HOUR			(1 << 5)

#تعریف_کرو HAVE_MDAY(s)		(s & S_MDAY)
#تعریف_کرو HAVE_MON(s)		(s & S_MON)
#تعریف_کرو HAVE_WDAY(s)		(s & S_WDAY)
#تعریف_کرو HAVE_YDAY(s)		(s & S_YDAY)
#تعریف_کرو HAVE_YEAR(s)		(s & S_YEAR)
#تعریف_کرو HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
ساکن ثابت حرف *ثابت nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ساکن ثابت حرف *ثابت nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ساکن ثابت صحیح آغاز_کا_month[2][13] = {
    /* non-leap year */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* leap year */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/*
 * Calculate the week day of the first day of a year. Valid for
 * the Gregorian calendar, which began Sept 14, 1752 in the UK
 * and its colonies. Ref:
 * http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
 */
    ساکن صحیح
پہلا_wday_کا(صحیح yr)
{
    واپس ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#تعریف_کرو delim(p)	((p) == '\0' || خلا_ہے((غیرعلامتی حرف)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ساکن صحیح
fromzone(ثابت غیرعلامتی حرف **bp غیر_مستعمل, ساخت tm *tm غیر_مستعمل,
						    صحیح mandatory غیر_مستعمل)
{
    واپس 0;
}

    حرف *
strptime(ثابت حرف *buf, ثابت حرف *fmt, ساخت tm *tm)
{
    غیرعلامتی حرف c;
    ثابت غیرعلامتی حرف *bp, *ep, *zname;
    صحیح alt_format, i, بانٹو_year = 0, neg = 0, state = 0,
	day_انحراف = -1, week_انحراف = 0, offs, mandatory;
    ثابت حرف *new_fmt;

    bp = (ثابت غیرعلامتی حرف *)buf;

    جب (bp != تہی && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	اگر (خلا_ہے(c))
	{
	    جب (خلا_ہے(*bp))
		bp++;
	    جاری;
	}

	اگر (c != '%')
	    جاؤ literal;


again:	انتخاب (c = *fmt++)
	{
	حالت '%':	/* "%%" is converted to "%". */
literal:
	    اگر (c != *bp++)
		واپس تہی;
	    LEGAL_ALT(0);
	    جاری;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	حالت 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    جاؤ again;

	حالت 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    جاؤ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	حالت 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    جاؤ recurse;

	حالت 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    جاؤ recurse;

	حالت 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    جاؤ recurse;

	حالت 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    جاؤ recurse;

	حالت 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    جاؤ recurse;

	حالت 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    جاؤ recurse;

	حالت 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    جاؤ recurse;

	حالت 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ثابت غیرعلامتی حرف *)strptime((ثابت حرف *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    جاری;

	/*
	 * "Elementary" conversion rules.
	 */
	حالت 'A':	/* The day of week, using the locale's form. */
	حالت 'a':
	    bp = ڈھونڈو_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    جاری;

	حالت 'B':	/* The month, using the locale's form. */
	حالت 'b':
	حالت 'h':
	    bp = ڈھونڈو_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    جاری;

	حالت 'C':	/* The century number. */
	    i = 20;
	    bp = conv_عدد(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_بنیاد;
	    اگر (بانٹو_year)
		i += tm->tm_year % 100;
	    بانٹو_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    جاری;

	حالت 'd':	/* The day of month. */
	حالت 'e':
	    bp = conv_عدد(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    جاری;

	حالت 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالت 'H':
	    bp = conv_عدد(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    جاری;

	حالت 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالت 'I':
	    bp = conv_عدد(bp, &tm->tm_hour, 1, 12);
	    اگر (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    جاری;

	حالت 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_عدد(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    جاری;

	حالت 'M':	/* The minute. */
	    bp = conv_عدد(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    جاری;

	حالت 'm':	/* The month. */
	    i = 1;
	    bp = conv_عدد(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    جاری;

	حالت 'p':	/* The locale's equivalent of AM/PM. */
	    bp = ڈھونڈو_string(bp, &i, c_am_pm, تہی, 2);
	    اگر (HAVE_HOUR(state) && tm->tm_hour > 11)
		واپس تہی;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    جاری;

	حالت 'S':	/* The seconds. */
	    bp = conv_عدد(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    جاری;

	حالت 's':	/* seconds since the epoch */
	    {
		ثابت time_t وقت_MAX = (time_t)((حجم(time_t) == 8)
					? LLONG_MAX : زیادہ_سے_زیادہ_عدد);
		time_t sse, d;

		اگر (*bp < '0' || *bp > '9')
		{
		    bp = تہی;
		    جاری;
		}

		sse = *bp++ - '0';
		جب (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    اگر (sse > وقت_MAX / 10)
		    {
			bp = تہی;
			توڑو;
		    }
		    sse *= 10;
		    اگر (sse > وقت_MAX - d)
		    {
			bp = تہی;
			توڑو;
		    }
		    sse += d;
		}
		اگر (bp == تہی)
		    جاری;

		{
		    ساخت tm *lt = localtime(&sse);

		    اگر (lt == تہی)
			bp = تہی;
		    ورنہ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		جاری;
	    }

	حالت 'U':	/* The week of year, beginning on sunday. */
	حالت 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_عدد(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    اگر (c == 'U')
		day_انحراف = TM_SUNDAY;
	    ورنہ
		day_انحراف = TM_MONDAY;
	    week_انحراف = i;
	    جاری;

	حالت 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_عدد(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    جاری;

	حالت 'u':	/* The day of week, monday = 1. */
	    bp = conv_عدد(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    جاری;

	حالت 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_عدد(bp, &i, 0, 99);
	    جاری;

	حالت 'G':	/* ISO week year with century (parsed, ignored). */
	    کرو
		bp++;
	    جب (ہندسہ_ہے(*bp));
	    جاری;

	حالت 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_عدد(bp, &i, 1, 53);
	    جاری;

	حالت 'Y':	/* The year. */
	    i = TM_YEAR_بنیاد;	/* just for data sanity... */
	    bp = conv_عدد(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_بنیاد;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    جاری;

	حالت 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_عدد(bp, &i, 0, 99);

	    اگر (بانٹو_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ورنہ
	    {
		بانٹو_year = 1;
		اگر (i <= 68)
		    i = i + 2000 - TM_YEAR_بنیاد;
		ورنہ
		    i = i + 1900 - TM_YEAR_بنیاد;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    جاری;

	حالت 'Z':
	حالت 'z':
	    tzset();
	    mandatory = c == 'z';
	    /*
	     * We recognize all ISO 8601 formats:
	     * Z	= Zulu time/UTC
	     * [+-]hhmm
	     * [+-]hh:mm
	     * [+-]hh
	     * We recognize all RFC-822/RFC-2822 formats:
	     * UT|GMT
	     *          North American : UTC offsets
	     * E[DS]T = Eastern : -4 | -5
	     * C[DS]T = Central : -5 | -6
	     * M[DS]T = Mountain: -6 | -7
	     * P[DS]T = Pacific : -7 | -8
	     *          Nautical/Military
	     * [A-IL-M] = -1 ... -9 (J not used)
	     * [N-Y]  = +1 ... +12
	     * Note: J maybe used to denote non-nautical
	     *       local time
	     */
	    اگر (mandatory)
		جب (خلا_ہے(*bp))
		    bp++;

	    zname = bp;
	    انتخاب (*bp++)
	    {
	    حالت 'G':
		اگر (*bp++ != 'M')
		    جاؤ namedzone;
		/*FALLTHROUGH*/
	    حالت 'U':
		اگر (*bp++ != 'T')
		    جاؤ namedzone;
		ورنہ اگر (!delim(*bp) && *bp++ != 'C')
		    جاؤ namedzone;
		/*FALLTHROUGH*/
	    حالت 'Z':
		اگر (!delim(*bp))
		    جاؤ namedzone;
		tm->tm_isdst = 0;
		جاری;
	    حالت '+':
		neg = 0;
		توڑو;
	    حالت '-':
		neg = 1;
		توڑو;
	    طےشدہ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		اگر (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    جاری;
		}
		/* 'J' is local time */
		اگر (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    جاری;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		اگر (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    جاؤ loadzone;
		ep = ڈھونڈو_string(bp, &i, nast, تہی, 4);
		اگر (ep != تہی)
		{
		    bp = ep;
		    جاری;
		}
		ep = ڈھونڈو_string(bp, &i, nadt, تہی, 4);
		اگر (ep != تہی)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    جاری;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#اگر_تعریف_شدہ USE_GET_TZNAME
		{
		    حرف tzbuf[2][32];
		    ثابت حرف *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    اگر (_get_tzname(&tzlen, tzbuf[0], حجم(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    اگر (_get_tzname(&tzlen, tzbuf[1], حجم(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = ڈھونڈو_string(bp, &i, tznames, تہی, 2);
		}
#ورنہ
		ep = ڈھونڈو_string(bp, &i,
			(ثابت حرف *ثابت *)tzname, تہی, 2);
#اگر_ختم
		اگر (ep != تہی)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    جاری;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		اگر (fromzone(&bp, tm, mandatory))
		    جاری;
		جاؤ برآمد;
	    }
	    offs = 0;
	    برائے (i = 0; i < 4; )
	    {
		اگر (ہندسہ_ہے(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    جاری;
		}
		اگر (i == 2 && *bp == ':')
		{
		    bp++;
		    جاری;
		}
		توڑو;
	    }
	    اگر (ہندسہ_ہے(*bp))
		جاؤ برآمد;
	    انتخاب (i)
	    {
	    حالت 2:
		offs *= SECSPERHOUR;
		توڑو;
	    حالت 4:
		i = offs % 100;
		offs /= 100;
		اگر (i >= SECSPERMIN)
		    جاؤ برآمد;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		توڑو;
	    طےشدہ:
برآمد:
		اگر (mandatory)
		    واپس تہی;
		bp = zname;
		جاری;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    اگر (offs >= (HOURSPERDAY * SECSPERHOUR))
		جاؤ برآمد;
	    اگر (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    جاری;

	/*
	 * Miscellaneous conversions.
	 */
	حالت 'n':	/* Any kind of white-space. */
	حالت 't':
	    جب (خلا_ہے(*bp))
		bp++;
	    LEGAL_ALT(0);
	    جاری;


	طےشدہ:	/* Unknown/unsupported conversion. */
	    واپس تہی;
	}
    }

    اگر (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	اگر (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  آغاز_کا_month[isleap_مجموع(tm->tm_year,
			    TM_YEAR_بنیاد)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ورنہ اگر (day_انحراف != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    اگر (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_انحراف;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    پہلا_wday_کا(tm->tm_year + TM_YEAR_بنیاد) +
		    day_انحراف) % 7 + (week_انحراف - 1) * 7 +
		    tm->tm_wday  - day_انحراف;
	    state |= S_YDAY;
	}
    }

    اگر (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	صحیح leap;

	اگر (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_مجموع(tm->tm_year, TM_YEAR_بنیاد);
	    جب (tm->tm_yday >= آغاز_کا_month[leap][i])
		i++;
	    اگر (i > 12)
	    {
		i = 1;
		tm->tm_yday -= آغاز_کا_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	اگر (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_مجموع(tm->tm_year, TM_YEAR_بنیاد);
	    tm->tm_mday = tm->tm_yday -
		آغاز_کا_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	اگر (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_انحراف = پہلا_wday_کا(tm->tm_year);
	    جب (i++ <= tm->tm_yday)
	    {
		اگر (week_انحراف++ >= 6)
		    week_انحراف = 0;
	    }
	    tm->tm_wday = week_انحراف;
	    state |= S_WDAY;
	}
    }

    واپس (حرف *)bp;
}


    ساکن ثابت غیرعلامتی حرف *
conv_عدد(ثابت غیرعلامتی حرف *buf, صحیح *dest, غیرعلامتی صحیح llim,
						    غیرعلامتی صحیح ulim)
{
    غیرعلامتی صحیح نتیجہ = 0;
    غیرعلامتی حرف ch;

    /* The limit also determines the number of valid digits. */
    غیرعلامتی صحیح rulim = ulim;

    ch = *buf;
    اگر (ch < '0' || ch > '9')
	واپس تہی;

    کرو
    {
	نتیجہ *= 10;
	نتیجہ += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } جب ((نتیجہ * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    اگر (نتیجہ < llim || نتیجہ > ulim)
	واپس تہی;

    *dest = نتیجہ;
    واپس buf;
}

    ساکن ثابت غیرعلامتی حرف *
ڈھونڈو_string(ثابت غیرعلامتی حرف *bp, صحیح *tgt, ثابت حرف *ثابت *n1,
					    ثابت حرف *ثابت *n2, صحیح c)
{
    صحیح		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    برائے (; n1 != تہی; n1 = n2, n2 = تہی)
    {
	برائے (i = 0; i < c; i++, n1++)
	{
	    len = لڑی_لمبائی(*n1);
	    اگر (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		واپس bp + len;
	    }
	}
    }

    /* Nothing matched */
    واپس تہی;
}

#اگر_ختم // MSWIN
