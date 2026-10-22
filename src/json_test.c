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
#അന്തര്ഭാവ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#അന്തര്ഭാവ "main.c"

// This file has to be included because the tested functions are static
#അന്തര്ഭാവ "json.c"

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    സ്ഥിതം ശൂന്യം
test_decode_find_end(ശൂന്യം)
{
    js_read_T reader;

    reader.js_fill = ശൂന്യ;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    പ്രതിജ്ഞാ(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == MAYBE);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    തിരികെ TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    സ്ഥിതം ശൂന്യം
test_fill_called_on_find_end(ശൂന്യം)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    പ്രതിജ്ഞാ(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    സ്ഥിതം ശൂന്യം
test_fill_called_on_string(ശൂന്യം)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    പ്രതിജ്ഞാ(json_decode_string(&reader, ശൂന്യ, '"') == OK);
}
#endif

    പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
#എങ്കിൽ defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    തിരികെ 0;
}
