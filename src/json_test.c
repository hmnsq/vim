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
#ଅନ୍ତର୍ଭାଵ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ଅନ୍ତର୍ଭାଵ "main.c"

// This file has to be included because the tested functions are static
#ଅନ୍ତର୍ଭାଵ "json.c"

#ଯଦି defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
test_decode_find_end(ଶୂନ୍ୟ)
{
    js_read_T reader;

    reader.js_fill = ଖାଲି;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == MAYBE);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
test_fill_called_on_find_end(ଶୂନ୍ୟ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    ପ୍ରତିଜ୍ଞା(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
test_fill_called_on_string(ଶୂନ୍ୟ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    ପ୍ରତିଜ୍ଞା(json_decode_string(&reader, ଖାଲି, '"') == OK);
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
#ଯଦି defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    ଫେରନ୍ତୁ 0;
}
