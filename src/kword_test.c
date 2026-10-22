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
#ಅನ್ತರ್ಭಾವ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ಅನ್ತರ್ಭಾವ "main.c"

// This file has to be included because the tested functions are static
#ಅನ್ತರ್ಭಾವ "charset.c"

/*
 * Test the results of vim_iswordc() and vim_iswordp() are matched.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_isword_funcs_utf8(ಶೂನ್ಯ)
{
    buf_T buf;
    ಪೂರ್ಣಾಙ್ಕ c;

    CLEAR_FIELD(buf);
    p_enc = (char_u *)"utf-8";
    p_isi = (char_u *)"";
    p_isp = (char_u *)"";
    p_isf = (char_u *)"";
    buf.b_p_isk = (char_u *)"@,48-57,_,128-167,224-235";

    curbuf = &buf;
    mb_init(); // calls init_chartab()

    ಪ್ರತಿ (c = 0; c < 0x10000; ++c)
    {
	char_u p[4] = {0};
	ಪೂರ್ಣಾಙ್ಕ c1;
	ಪೂರ್ಣಾಙ್ಕ retc;
	ಪೂರ್ಣಾಙ್ಕ retp;

	utf_char2bytes(c, p);
	c1 = utf_ptr2char(p);
	ಯದಿ (c != c1)
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
	ಯದಿ (retc != retp)
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

    ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
    estack_init();
    test_isword_funcs_utf8();
    ಪ್ರತ್ಯಾಗಮ 0;
}
