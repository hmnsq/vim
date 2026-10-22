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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EnhancedBP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EnhancedBP_H

#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/PushBP.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_ਧੱਕੋ_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


ਬਾਹਰੀ XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _XmEnhancedButtonPart
{
    // public resources
    String pixmap_ਅੰਕੜੇ;
    String pixmap_file;
    Dimension spacing;
    ਪੂਰਨ label_location;

    // private resources
    ਪੂਰਨ pixmap_ਡੂੰਘਾਈ;
    Dimension pixmap_ਚੌੜਾਈ;
    Dimension pixmap_ਉਚਾਈ;
    Pixmap ਸਾਧਾਰਨ_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    ਪੂਰਨ doing_setvalues;
    ਪੂਰਨ doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_ਧੱਕੋ_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#ਜੇ_ਅੰਤ
