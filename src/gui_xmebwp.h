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

#اگر_تعریف_نشده EnhancedBP_H
#تعریف_کن EnhancedBP_H

#ضمن <Xm/PushBP.h>

#ضمن "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
تعريفنوع بنية _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
تعريفنوع بنية
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_افزودن_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


خارجي XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
تعريفنوع بنية _XmEnhancedButtonPart
{
    // public resources
    String pixmap_داده;
    String pixmap_file;
    Dimension spacing;
    صحيح label_location;

    // private resources
    صحيح pixmap_عمق;
    Dimension pixmap_عرض;
    Dimension pixmap_ارتفاع;
    Pixmap نرمال_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    صحيح doing_setvalues;
    صحيح doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
تعريفنوع بنية _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_افزودن_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#پایان_اگر
