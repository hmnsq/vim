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
#ಅನ್ತರ್ಭಾವ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ಅನ್ತರ್ಭಾವ "main.c"

// This file has to be included because the tested functions are static
#ಅನ್ತರ್ಭಾವ "json.c"

#ಯದಿ defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_decode_find_end(ಶೂನ್ಯ)
{
    js_read_T reader;

    reader.js_fill = ರಿಕ್ತ;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == MAYBE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_fill_called_on_find_end(ಶೂನ್ಯ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    ಪ್ರತಿಜ್ಞಾ(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_fill_called_on_string(ಶೂನ್ಯ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    ಪ್ರತಿಜ್ಞಾ(json_decode_string(&reader, ರಿಕ್ತ, '"') == OK);
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
#ಯದಿ defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    ಪ್ರತ್ಯಾಗಮ 0;
}
