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

#ifndef EnhancedBP_H
#define EnhancedBP_H

#ଅନ୍ତର୍ଭାଵ <Xm/PushBP.h>

#ଅନ୍ତର୍ଭାଵ "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_ଠେଲ_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


ବାହ୍ୟ XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _XmEnhancedButtonPart
{
    // public resources
    String pixmap_data;
    String pixmap_file;
    Dimension spacing;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_location;

    // private resources
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pixmap_depth;
    Dimension pixmap_width;
    Dimension pixmap_height;
    Pixmap normal_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା doing_setvalues;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_ଠେଲ_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#endif
