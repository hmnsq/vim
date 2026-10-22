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

#నిర్వచనం_రద్దు NDEBUG
#అన్తర్భావ <assert.h>

// Must include main.c because it contains much more than just main()
#నిర్వచించు NO_VIM_MAIN
#అన్తర్భావ "main.c"

// This file has to be included because the tested functions are static
#అన్తర్భావ "json.c"

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Test json_find_end() with incomplete items.
 */
    స్థిర శూన్య
test_decode_కనుగొను_చివర(శూన్య)
{
    js_read_T reader;

    reader.js_fill = ఖాళీ;
    reader.js_వాడిన = 0;

    // string and incomplete string
    reader.js_buf = (char_u *)"\"hello\"";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  \"hello\" ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"\"hello";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // number and dash (incomplete number)
    reader.js_buf = (char_u *)"123";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"-";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // false, true and null, also incomplete
    reader.js_buf = (char_u *)"false";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"f";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fa";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fal";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"fals";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"true";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"t";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tr";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"tru";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    reader.js_buf = (char_u *)"null";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"n";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nu";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"nul";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // object without white space
    reader.js_buf = (char_u *)"{\"a\":123}";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"{\"a\":123";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\":";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a\"";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"a";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{\"";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"{";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // object with white space
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  }  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  {  \"a\"  :  123  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  :  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a\"  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {  \"a  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  {   ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // JS object with white space
    reader.js_buf = (char_u *)"  {  a  :  123  }  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, JSON_JS) == OK);
    reader.js_buf = (char_u *)"  {  a  :   ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, JSON_JS) == MAYBE);

    // array without white space
    reader.js_buf = (char_u *)"[\"a\",123]";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"[\"a\",123";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\",";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a\"";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"a";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[\"";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"[";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);

    // array with white space
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ]  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  \"a  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
    reader.js_buf = (char_u *)"  [  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == MAYBE);
}

    స్థిర పూర్ణాంకం
fill_from_cookie(js_read_T *reader)
{
    reader->js_buf = reader->js_cookie;
    తిరిగి TRUE;
}

/*
 * Test json_find_end with an incomplete array, calling the fill function.
 */
    స్థిర శూన్య
test_fill_called_on_కనుగొను_చివర(శూన్య)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_వాడిన = 0;
    reader.js_buf = (char_u *)"  [  \"a\"  ,  123  ";
    reader.js_cookie =	      "  [  \"a\"  ,  123  ]  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ,  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a\"  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  \"a";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
    reader.js_buf = (char_u *)"  [  ";
    ప్రతిజ్ఞా(json_కనుగొను_చివర(&reader, 0) == OK);
}

/*
 * Test json_find_end with an incomplete string, calling the fill function.
 */
    స్థిర శూన్య
test_fill_called_on_string(శూన్య)
{
    js_read_T reader;

    reader.js_fill = fill_from_cookie;
    reader.js_వాడిన = 0;
    reader.js_buf = (char_u *)" \"foo";
    reader.js_చివర = reader.js_buf + STRLEN(reader.js_buf);
    reader.js_cookie =	      " \"foobar\"  ";
    ప్రతిజ్ఞా(json_decode_string(&reader, ఖాళీ, '"') == OK);
}
#అయితే_ముగింపు

    పూర్ణాంకం
ప్రధాన(శూన్య)
{
#ఒకవేళ defined(FEAT_EVAL)
    p_mfd = 100;
    test_decode_కనుగొను_చివర();
    test_fill_called_on_కనుగొను_చివర();
    test_fill_called_on_string();
#అయితే_ముగింపు
    తిరిగి 0;
}
