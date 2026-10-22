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

#که_نه_تعریف_شوی EnhancedB_H
#تعریف_کړه EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#که_نه_تعریف_شوی XmNshift
# define XmNshift		"shift"
#د_که_پای
#که_نه_تعریف_شوی XmCShift
# define XmCShift		"Shift"
#د_که_پای

#که_نه_تعریف_شوی XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#د_که_پای
#که_نه_تعریف_شوی XmCLocation
# define XmCLocation		"Location"
#د_که_پای

#که_نه_تعریف_شوی Xm_Npixmap_معلومات
# define Xm_Npixmap_معلومات		"pixmapData"
#د_که_پای

#که_نه_تعریف_شوی XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#د_که_پای

/*
 * Constants for labelLocation.
 */
#که_تعریف_شوی HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#وإلا
# define Xm_کیڼ	    1
# define Xm_ښی    2
# define Xm_سر	    3
# define XmBOTTOM   4
#د_که_پای

#تعریف_کړه XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
خارجي Widget XgCreateEPushButtonWidget(Widget, حرف *, ArgList, Cardinal);

خارجي WidgetClass xmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#د_که_پای
