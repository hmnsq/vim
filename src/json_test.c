/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * json_test.c: Unittests for json.c
 */

#undef NDEBUG
#અન્તર્ભાવ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#અન્તર્ભાવ "main.c"

// This file has to be included because the tested functions are static
#અન્તર્ભાવ "json.c"

#જો defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    સ્થિર શૂન્ય
test_decode_find_end(શૂન્ય)
{
    js_read_T reader;

    reader.js_fill = ખાલી;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    પ્રતિજ્ઞા(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    પ્રતિજ્ઞા(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == MAYBE);
}

    સ્થિર પૂર્ણાંક
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    પાછા TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    સ્થિર શૂન્ય
test_fill_called_on_find_end(શૂન્ય)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    પ્રતિજ્ઞા(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    સ્થિર શૂન્ય
test_fill_called_on_string(શૂન્ય)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    પ્રતિજ્ઞા(json_decode_string(&reader, ખાલી, '"') == OK);
}
#endif

    પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
#જો defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    પાછા 0;
}
