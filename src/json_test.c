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
#அந்தர்பாவ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#அந்தர்பாவ "main.c"

// This file has to be included because the tested functions are static
#அந்தர்பாவ "json.c"

#எனில் defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    நிலையான வெறுமை
test_decode_find_end(வெறுமை)
{
    js_read_T reader;

    reader.js_fill = வெற்று;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == MAYBE);
}

    நிலையான முழு
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    திரும்பு TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    நிலையான வெறுமை
test_fill_called_on_find_end(வெறுமை)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    ப்ரதிஜ்ஞா(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    நிலையான வெறுமை
test_fill_called_on_string(வெறுமை)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    ப்ரதிஜ்ஞா(json_decode_string(&reader, வெற்று, '"') == OK);
}
#endif

    முழு
முதன்மை(வெறுமை)
{
#எனில் defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    திரும்பு 0;
}
