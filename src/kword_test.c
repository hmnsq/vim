/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * kword_test.c: Unittests for vim_iswordc() and vim_iswordp().
 */

#undef NDEBUG
#অন্তর্ভাব <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#অন্তর্ভাব "main.c"

// This file has to be included because the tested functions are static
#অন্তর্ভাব "charset.c"

/*
 * Test the results of vim_iswordc() and vim_iswordp() are matched.
 */
    স্থির শূন্য
test_isword_funcs_utf8(শূন্য)
{
    buf_T buf;
    পূর্ণাঙ্ক c;

    CLEAR_FIELD(buf);
    p_enc = (char_u *)"utf-8";
    p_isi = (char_u *)"";
    p_isp = (char_u *)"";
    p_isf = (char_u *)"";
    buf.b_p_isk = (char_u *)"@,48-57,_,128-167,224-235";

    curbuf = &buf;
    mb_init(); // calls init_chartab()

    প্রতি (c = 0; c < 0x10000; ++c)
    {
	char_u p[4] = {0};
	পূর্ণাঙ্ক c1;
	পূর্ণাঙ্ক retc;
	পূর্ণাঙ্ক retp;

	utf_char2bytes(c, p);
	c1 = utf_ptr2char(p);
	যদি (c != c1)
	{
	    fprintf(stderr, "Failed: ");
	    fprintf(stderr,
		    "[c = %#04x, p = {%#02x, %#02x, %#02x}] ",
		    c, p[0], p[1], p[2]);
	    fprintf(stderr, "c != utf_ptr2char(p) (=%#04x)\n", c1);
	    abort();
	}
	retc = vim_iswordc_buf(c, &buf);
	retp = vim_iswordp_buf(p, &buf);
	যদি (retc != retp)
	{
	    fprintf(stderr, "Failed: ");
	    fprintf(stderr,
		    "[c = %#04x, p = {%#02x, %#02x, %#02x}] ",
		    c, p[0], p[1], p[2]);
	    fprintf(stderr, "vim_iswordc(c) (=%d) != vim_iswordp(p) (=%d)\n",
		    retc, retp);
	    abort();
	}
    }
}

    পূর্ণাঙ্ক
main(শূন্য)
{
    estack_init();
    test_isword_funcs_utf8();
    প্রত্যাগম 0;
}
