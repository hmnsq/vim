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

#اگر_غیر_تعریف_شدہ EnhancedB_H
#تعریف_کرو EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#اگر_غیر_تعریف_شدہ XmNshift
# define XmNshift		"shift"
#اگر_ختم
#اگر_غیر_تعریف_شدہ XmCShift
# define XmCShift		"Shift"
#اگر_ختم

#اگر_غیر_تعریف_شدہ XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#اگر_ختم
#اگر_غیر_تعریف_شدہ XmCLocation
# define XmCLocation		"Location"
#اگر_ختم

#اگر_غیر_تعریف_شدہ Xm_Npixmap_ڈیٹا
# define Xm_Npixmap_ڈیٹا		"pixmapData"
#اگر_ختم

#اگر_غیر_تعریف_شدہ XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#اگر_ختم

/*
 * Constants for labelLocation.
 */
#اگر_تعریف_شدہ HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#ورنہ
# define Xm_بایاں	    1
# define Xm_دایاں    2
# define Xm_چوٹی	    3
# define XmBOTTOM   4
#اگر_ختم

#تعریف_کرو XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
خارجی Widget XgCreateEPushButtonWidget(Widget, حرف *, ArgList, Cardinal);

خارجی WidgetClass xmEnhancedButtonWidgetClass;
قسمتعریف ساخت _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
قسمتعریف ساخت _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#اگر_ختم
