/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * message_test.c: Unittests for message.c
 */

#undef NDEBUG
#अन्तर्भाव <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#अन्तर्भाव "main.c"

// This file has to be included because some of the tested functions are
// static.
#अन्तर्भाव "message.c"

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

// These formats are not standard in C printf() function.
// Use a global variable rather than a literal format to disable
// -Wformat compiler warnings:
//
// - warning: '0' flag used with ‘%p’ gnu_printf format
// - warning: format ‘%S’ expects argument of type ‘wchar_t *’, but argument 4 has type ‘char *’
// - warning: unknown conversion type character ‘b’ in format
//
// These formats are in practise only used from Vim script printf()
// function and never as literals in C code.
वर्ण *fmt_012p = "%012p";
वर्ण *fmt_5S   = "%5S";
वर्ण *fmt_06b  = "%06b";
वर्ण *fmt_06pb = "%1$0.*2$b";
वर्ण *fmt_06pb2 = "%2$0*1$b";
वर्ण *fmt_212s = "%2$s %1$s %2$s";
वर्ण *fmt_21s  = "%2$s %1$s";

/*
 * Test trunc_string().
 */
    स्थिर शून्य
test_trunc_string(शून्य)
{
    char_u  *buf; /*allocated every time to find uninit errors */
    char_u  *s;

    // Should not write anything to destination if buflen is 0.
    trunc_string((char_u *)"", रिक्त, 1, 0);

    // Truncating an empty string does nothing.
    buf = alloc(1);
    trunc_string((char_u *)"", buf, 1, 1);
    प्रतिज्ञा(buf[0] == NUL);
    vim_free(buf);

    // in place
    buf = alloc(40);
    STRCPY(buf, "text");
    trunc_string(buf, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "text") == 0);
    vim_free(buf);

    buf = alloc(40);
    STRCPY(buf, "a short text");
    trunc_string(buf, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a short text") == 0);
    vim_free(buf);

    buf = alloc(40);
    STRCPY(buf, "a text tha just fits");
    trunc_string(buf, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text tha just fits") == 0);
    vim_free(buf);

    buf = alloc(40);
    STRCPY(buf, "a text that nott fits");
    trunc_string(buf, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text t...nott fits") == 0);
    vim_free(buf);

    // copy from string to buf
    buf = alloc(40);
    s = vim_strsave((char_u *)"text");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "text") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a text that fits");
    trunc_string(s, buf, 34, 40);
    प्रतिज्ञा(STRCMP(buf, "a text that fits") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a short text");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a short text") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a text tha just fits");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text tha just fits") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a text that nott fits");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text t...nott fits") == 0);
    vim_free(buf);
    vim_free(s);
}

/*
 * Test trunc_string() with mbyte chars.
 */
    स्थिर शून्य
test_trunc_string_mbyte(शून्य)
{
    char_u  *buf; // allocated every time to find uninit errors
    char_u  *s;

    buf = alloc(40);
    s = vim_strsave((char_u *)"Ä text tha just fits");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "Ä text tha just fits") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a text ÄÖÜä nott fits");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text Ä...nott fits") == 0);
    vim_free(buf);
    vim_free(s);

    buf = alloc(40);
    s = vim_strsave((char_u *)"a text that not fitsÄ");
    trunc_string(s, buf, 20, 40);
    प्रतिज्ञा(STRCMP(buf, "a text t...not fitsÄ") == 0);
    vim_free(buf);
    vim_free(s);
}

/*
 * Test vim_snprintf() with a focus on checking that truncation is
 * correct when buffer is small, since it cannot be tested from
 * Vim script tests. Check that:
 * - no buffer overflows happens (with valgrind or asan)
 * - output string is always NUL terminated.
 *
 * Not all formats of vim_snprintf() are checked here. They are
 * checked more exhaustively in Test_printf*() Vim script tests.
 */
    स्थिर शून्य
test_vim_snprintf(शून्य)
{
    पूर्णाङ्क		n;
    size_t	bsize;
    पूर्णाङ्क		bsize_int;
    शून्य	*ptr = (शून्य *)0x87654321;

    // Loop on various buffer sizes to make sure that truncation of
    // vim_snprintf() is correct.
    प्रति (bsize = 0; bsize < 15; ++bsize)
    {
	bsize_int = (पूर्णाङ्क)bsize - 1;

	// buf is the heap rather than in the stack
	// so valgrind can detect buffer overflows if any.
	// Use malloc() rather than alloc() as test checks with 0-size
	// buffer and its content should then never be used.
	वर्ण *buf = malloc(bsize);

	n = vim_snprintf(buf, bsize, "%.8g", 10000000.1);
	प्रतिज्ञा(n == 12);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1.00000001e7", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%d", 1234567);
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%ld", 1234567L);
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%9ld", 1234567L);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "  1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%-9ld", 1234567L);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%x", 0xdeadbeef);
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "deadbeef", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_06b, (uvarnumber_T)12);
	प्रतिज्ञा(n == 6);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "001100", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%s %s", "one", "two");
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "one two", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%f", 1.234);
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1.234000", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%e", 1.234);
	प्रतिज्ञा(n == 12);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1.234000e+00", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%f", 0.0/0.0);
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "nan", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%f", 1.0/0.0);
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "inf", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%f", -1.0/0.0);
	प्रतिज्ञा(n == 4);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "-inf", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%f", -0.0);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "-0.000000", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%s", "漢語");
	प्रतिज्ञा(n == 6);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "漢語", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%8s", "漢語");
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "  漢語", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%-8s", "漢語");
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "漢語  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%.3s", "漢語");
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "漢", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_5S, "foo");
	प्रतिज्ञा(n == 5);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "  foo", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%%%%%%");
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "%%%", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%c%c", 1, 2);
	प्रतिज्ञा(n == 2);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "\x01\x02", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	// %p format is not tested in Vim script tests Test_printf*()
	// as it only makes sense in C code.
	// NOTE: SunOS libc doesn't use the prefix "0x" on %p.
#ifdef SUN_SYSTEM
# define PREFIX_LEN  0
# define PREFIX_STR1 ""
# define PREFIX_STR2 "00"
#अन्यथा
# define PREFIX_LEN  2
# define PREFIX_STR1 "0x"
# define PREFIX_STR2 "0x"
#endif
	n = vim_snprintf(buf, bsize, "%p", ptr);
	प्रतिज्ञा(n == 8 + PREFIX_LEN);
	प्रतिज्ञा(bsize == 0
		     || STRNCMP(buf, PREFIX_STR1 "87654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_012p, ptr);
	प्रतिज्ञा(n == 12);
	प्रतिज्ञा(bsize == 0
		   || STRNCMP(buf, PREFIX_STR2 "0087654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	मुक्त(buf);
    }
}

/*
 * Test vim_snprintf() with a focus on checking that positional
 * arguments are correctly applied and skipped
 */
    स्थिर शून्य
test_vim_snprintf_positional(शून्य)
{
    पूर्णाङ्क		n;
    size_t	bsize;
    पूर्णाङ्क		bsize_int;

    // Loop on various buffer sizes to make sure that truncation of
    // vim_snprintf() is correct.
    प्रति (bsize = 0; bsize < 25; ++bsize)
    {
	bsize_int = (पूर्णाङ्क)bsize - 1;

	// buf is the heap rather than in the stack
	// so valgrind can detect buffer overflows if any.
	// Use malloc() rather than alloc() as test checks with 0-size
	// buffer and its content should then never be used.
	वर्ण *buf = malloc(bsize);

	n = vim_snprintf(buf, bsize, "%1$*2$ld", 1234567L, -9);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$*2$.*3$ld", 1234567L, -9, 5);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$*3$.*2$ld", 1234567L, 5, -9);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%3$*1$.*2$ld", -9, 5, 1234567L);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567  ", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$ld", 1234567L);
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$*2$ld", 1234567L, 9);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "  1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$ld %1$d %3$lu", 12345, 9L, 7654321UL);
	प्रतिज्ञा(n == 15);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 12345 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$d %1$ld %3$lu", 1234567L, 9, 7654321UL);
	प्रतिज्ञा(n == 17);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 1234567 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$d %1$lld %3$lu", 1234567LL, 9, 7654321UL);
	प्रतिज्ञा(n == 17);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 1234567 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$ld %1$u %3$lu", 12345U, 9L, 7654321UL);
	प्रतिज्ञा(n == 15);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 12345 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$d %1$lu %3$lu", 1234567UL, 9, 7654321UL);
	प्रतिज्ञा(n == 17);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 1234567 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$d %1$llu %3$lu", 1234567LLU, 9, 7654321UL);
	प्रतिज्ञा(n == 17);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 1234567 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$d %1$x %3$lu", 0xdeadbeef, 9, 7654321UL);
	प्रतिज्ञा(n == 18);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 deadbeef 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$ld %1$c %3$lu", 'c', 9L, 7654321UL);
	प्रतिज्ञा(n == 11);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 c 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$ld %1$s %3$lu", "hi", 9L, 7654321UL);
	प्रतिज्ञा(n == 12);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 hi 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%2$ld %1$e %3$lu", 0.0, 9L, 7654321UL);
	प्रतिज्ञा(n == 22);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "9 0.000000e+00 7654321", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_212s, "one", "two", "three");
	प्रतिज्ञा(n == 11);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "two one two", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%3$s %1$s %2$s", "one", "two", "three");
	प्रतिज्ञा(n == 13);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "three one two", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$d", 1234567);
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1234567", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$x", 0xdeadbeef);
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "deadbeef", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_06pb2, 6, (uvarnumber_T)12);
	प्रतिज्ञा(n == 6);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "001100", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_06pb, (uvarnumber_T)12, 6);
	प्रतिज्ञा(n == 6);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "001100", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$s %2$s", "one", "two");
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "one two", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_06b, (uvarnumber_T)12);
	प्रतिज्ञा(n == 6);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "001100", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, fmt_21s, "one", "two", "three");
	प्रतिज्ञा(n == 7);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "two one", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

#ifdef FEAT_FLOAT
	n = vim_snprintf(buf, bsize, "%1$f", 1.234);
	प्रतिज्ञा(n == 8);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1.234000", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$e", 1.234);
	प्रतिज्ञा(n == 12);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "1.234000e+00", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$f", 0.0/0.0);
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "nan", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$f", 1.0/0.0);
	प्रतिज्ञा(n == 3);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "inf", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$f", -1.0/0.0);
	प्रतिज्ञा(n == 4);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "-inf", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');

	n = vim_snprintf(buf, bsize, "%1$f", -0.0);
	प्रतिज्ञा(n == 9);
	प्रतिज्ञा(bsize == 0 || STRNCMP(buf, "-0.000000", bsize_int) == 0);
	प्रतिज्ञा(bsize == 0 || buf[MIN(n, bsize_int)] == '\0');
#endif

	मुक्त(buf);
    }
}

    पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
    CLEAR_FIELD(params);
    params.argc = argc;
    params.argv = argv;
    common_init_1();
    common_init_2(&params);

    set_option_मूल्य_give_err((char_u *)"encoding", 0, (char_u *)"utf-8", 0);
    init_chartab();
    test_trunc_string();
    test_trunc_string_mbyte();
    test_vim_snprintf();
    test_vim_snprintf_positional();

    set_option_मूल्य_give_err((char_u *)"encoding", 0, (char_u *)"latin1", 0);
    init_chartab();
    test_trunc_string();
    test_vim_snprintf();
    test_vim_snprintf_positional();

    प्रत्यागम 0;
}
