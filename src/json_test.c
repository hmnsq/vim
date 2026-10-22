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
#অন্তর্ভাব <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#অন্তর্ভাব "main.c"

// This file has to be included because the tested functions are static
#অন্তর্ভাব "json.c"

#যদি defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    স্থির শূন্য
test_decode_find_end(শূন্য)
{
    js_read_T reader;

    reader.js_fill = ফাঁকা;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    প্রতিজ্ঞা(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    প্রতিজ্ঞা(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == MAYBE);
}

    স্থির পূর্ণসংখ্যা
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    ফেরত TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    স্থির শূন্য
test_fill_called_on_find_end(শূন্য)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    প্রতিজ্ঞা(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    স্থির শূন্য
test_fill_called_on_string(শূন্য)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    প্রতিজ্ঞা(json_decode_string(&reader, ফাঁকা, '"') == OK);
}
#endif

    পূর্ণসংখ্যা
প্রধান(শূন্য)
{
#যদি defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    ফেরত 0;
}
