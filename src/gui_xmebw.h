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

#اگر_تعریف_نشده EnhancedB_H
#تعریف_کن EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#اگر_تعریف_نشده XmNshift
# define XmNshift		"shift"
#پایان_اگر
#اگر_تعریف_نشده XmCShift
# define XmCShift		"Shift"
#پایان_اگر

#اگر_تعریف_نشده XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#پایان_اگر
#اگر_تعریف_نشده XmCLocation
# define XmCLocation		"Location"
#پایان_اگر

#اگر_تعریف_نشده Xm_Npixmap_داده
# define Xm_Npixmap_داده		"pixmapData"
#پایان_اگر

#اگر_تعریف_نشده XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#پایان_اگر

/*
 * Constants for labelLocation.
 */
#اگر_تعریف_شده HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#وإلا
# define Xm_چپ	    1
# define Xm_راست    2
# define Xm_بالاترین	    3
# define XmBOTTOM   4
#پایان_اگر

#تعریف_کن XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
خارجي Widget XgCreateEPushButtonWidget(Widget, حرف *, ArgList, Cardinal);

خارجي WidgetClass xmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#پایان_اگر
