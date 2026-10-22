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

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ NDEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

// Must include main.c because it contains much more than just main()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_VIM_MAIN
#ਅਨ੍ਤਰ੍ਭਾਵ "main.c"

// This file has to be included because the tested functions are static
#ਅਨ੍ਤਰ੍ਭਾਵ "json.c"

#ਜੇ defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    ਸਥਿਰ ਖਾਲੀ
test_decode_ਲੱਭੋ_ਸਿਰਾ(ਖਾਲੀ)
{
    js_read_T reader;

    reader.js_fill = ਨਿਰਾ;
    reader.js_ਵਰਤਿਆ = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == MAYBE);
}

    ਸਥਿਰ ਪੂਰਨ
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    ਵਾਪਸ TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    ਸਥਿਰ ਖਾਲੀ
test_fill_called_on_ਲੱਭੋ_ਸਿਰਾ(ਖਾਲੀ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_ਵਰਤਿਆ = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_ਲੱਭੋ_ਸਿਰਾ(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    ਸਥਿਰ ਖਾਲੀ
test_fill_called_on_string(ਖਾਲੀ)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_ਵਰਤਿਆ = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_ਸਿਰਾ = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    ਪ੍ਰਤਿਜ੍ਞਾ(json_decode_string(&reader, ਨਿਰਾ, '"') == OK);
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_ਲੱਭੋ_ਸਿਰਾ();
    test_fill_called_on_ਲੱਭੋ_ਸਿਰਾ();
    test_fill_called_on_string();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ 0;
}
