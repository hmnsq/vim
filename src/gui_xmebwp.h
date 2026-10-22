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

#నిర్వచితం_కాకపోతే EnhancedBP_H
#నిర్వచించు EnhancedBP_H

#అన్తర్భావ <Xm/PushBP.h>

#అన్తర్భావ "gui_xmebw.h"


/*
 * EnhancedButton class structure.
 */
రకనిర్వచనం నిర్మాణం _XmEnhancedButtonClassPart
{
    Pixmap stipple_bitmap;
} XmEnhancedButtonClassPart;

/*
 * Full class record declaration for EnhancedButton class.
 */
రకనిర్వచనం నిర్మాణం
{
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmLabelClassPart label_class;
    Xm_నెట్టు_Button_Class_Part pushbutton_class;
    XmEnhancedButtonClassPart enhancedbutton_class;
} XmEnhancedButtonClassRec;


బాహ్య XmEnhancedButtonClassRec xmEnhancedButtonClassRec;

/*
 * EnhancedButton instance record.
 */
రకనిర్వచనం నిర్మాణం _XmEnhancedButtonPart
{
    // public resources
    String pixmap_దత్తాంశం;
    String pixmap_file;
    Dimension spacing;
    పూర్ణాంకం label_location;

    // private resources
    పూర్ణాంకం pixmap_లోతు;
    Dimension pixmap_వెడల్పు;
    Dimension pixmap_ఎత్తు;
    Pixmap సాధారణ_pixmap;
    Pixmap armed_pixmap;
    Pixmap insensitive_pixmap;
    Pixmap highlight_pixmap;

    పూర్ణాంకం doing_setvalues;
    పూర్ణాంకం doing_destroy;
} XmEnhancedButtonPart;


/*
 * Full instance record declaration.
 */
రకనిర్వచనం నిర్మాణం _XmEnhancedButtonRec
{
    CorePart core;
    XmPrimitivePart primitive;
    XmLabelPart label;
    Xm_నెట్టు_Button_Part pushbutton;
    XmEnhancedButtonPart enhancedbutton;
} XmEnhancedButtonRec;

#అయితే_ముగింపు
