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

#جے_بنا_تعریف EnhancedB_H
#تعریف_کرو EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#جے_بنا_تعریف XmNshift
# define XmNshift		"shift"
#جے_انت
#جے_بنا_تعریف XmCShift
# define XmCShift		"Shift"
#جے_انت

#جے_بنا_تعریف XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#جے_انت
#جے_بنا_تعریف XmCLocation
# define XmCLocation		"Location"
#جے_انت

#جے_بنا_تعریف Xm_Npixmap_انکڑے
# define Xm_Npixmap_انکڑے		"pixmapData"
#جے_انت

#جے_بنا_تعریف XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#جے_انت

/*
 * Constants for labelLocation.
 */
#جے_تعریف_شدہ HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#وإلا
# define Xm_بایاں	    1
# define Xm_دایاں    2
# define Xm_چوٹی	    3
# define XmBOTTOM   4
#جے_انت

#تعریف_کرو XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
خارجي Widget XgCreateEPushButtonWidget(Widget, حرف *, ArgList, Cardinal);

خارجي WidgetClass xmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#جے_انت
