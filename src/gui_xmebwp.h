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

#अन्तर्भाव <Xm/PushBP.h>

#अन्तर्भाव "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
प्रकारनिर्देश संरचना _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
प्रकारनिर्देश संरचना
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_ढकला_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


बाह्य XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
प्रकारनिर्देश संरचना _XmEnhancedButtonPart
{
    // public resources
    String pixmap_data;
    String pixmap_file;
    Dimension spacing;
    पूर्णाङ्क label_location;

    // private resources
    पूर्णाङ्क pixmap_depth;
    Dimension pixmap_width;
    Dimension pixmap_height;
    Pixmap normal_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    पूर्णाङ्क doing_setvalues;
    पूर्णाङ्क doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
प्रकारनिर्देश संरचना _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_ढकला_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#endif
