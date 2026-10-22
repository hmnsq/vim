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
#ضمن <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ضمن "main.c"

// This file has to be included because the tested functions are static
#ضمن "json.c"

#ܐܢ defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    ساكن فراغ
test_decode_find_end(فراغ)
{
    js_read_T reader;

    reader.js_fill = عدم;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    أكد(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    أكد(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    أكد(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    أكد(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    أكد(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    أكد(json_find_end(&reader, 0) == MAYBE);
}

    ساكن صحيح
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    ܦܢܐ TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    ساكن فراغ
test_fill_called_on_find_end(فراغ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    أكد(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    أكد(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    ساكن فراغ
test_fill_called_on_string(فراغ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    أكد(json_decode_string(&reader, عدم, '"') == OK);
}
#endif

    صحيح
main(فراغ)
{
#ܐܢ defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    ܦܢܐ 0;
}
