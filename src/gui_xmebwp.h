/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 *
 * (C) 2002,2005 by Marcin Dalecki <martin@dalecki.de>
 *
 * MARCIN DALECKI ASSUMES NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY
 * OF THIS SOFTWARE . THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, AND MARCIN DALECKI EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 */

#اگر_غیر_تعریف_شدہ EnhancedBP_H
#تعریف_کرو EnhancedBP_H

#شامل <Xm/PushBP.h>

#شامل "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
قسمتعریف ساخت _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
قسمتعریف ساخت
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_ڈالو_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


خارجی XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
قسمتعریف ساخت _XmEnhancedButtonPart
{
    // public resources
    String pixmap_ڈیٹا;
    String pixmap_file;
    Dimension spacing;
    صحیح label_location;

    // private resources
    صحیح pixmap_گہرائی;
    Dimension pixmap_چوڑائی;
    Dimension pixmap_اونچائی;
    Pixmap معمول_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    صحیح doing_setvalues;
    صحیح doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
قسمتعریف ساخت _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_ڈالو_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#اگر_ختم
