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
#अन्तर्भाव <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#अन्तर्भाव "main.c"

// This file has to be included because the tested functions are static
#अन्तर्भाव "json.c"

#यदि defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    स्थिर शून्य
test_decode_find_end(शून्य)
{
    js_read_T reader;

    reader.js_fill = रिक्त;
    reader.js_used = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    प्रतिज्ञा(json_find_end(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    प्रतिज्ञा(json_find_end(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == MAYBE);
}

    स्थिर पूर्णाङ्क
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    प्रत्यागम TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    स्थिर शून्य
test_fill_called_on_find_end(शून्य)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    प्रतिज्ञा(json_find_end(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    स्थिर शून्य
test_fill_called_on_string(शून्य)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_used = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_end = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    प्रतिज्ञा(json_decode_string(&reader, रिक्त, '"') == OK);
}
#endif

    पूर्णाङ्क
main(शून्य)
{
#यदि defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_find_end();
    test_fill_called_on_find_end();
    test_fill_called_on_string();
#endif
    प्रत्यागम 0;
}
