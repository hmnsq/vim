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

#جيڪڏهن_وصف_نه_ٿيل EnhancedB_H
#وصف_ڪريو EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#جيڪڏهن_وصف_نه_ٿيل XmNshift
# define XmNshift		"shift"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل XmCShift
# define XmCShift		"Shift"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل XmCLocation
# define XmCLocation		"Location"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل Xm_Npixmap_معلومات
# define Xm_Npixmap_معلومات		"pixmapData"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#جيڪڏهن_پورو

/*
 * Constants for labelLocation.
 */
#جيڪڏهن_وصف_ٿيل HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#وإلا
# define Xm_کاٻو	    1
# define Xm_ساڄو    2
# define Xm_چوٽي	    3
# define XmBOTTOM   4
#جيڪڏهن_پورو

#وصف_ڪريو XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
خارجي Widget XgCreateEPushButtonWidget(Widget, حرف *, ArgList, Cardinal);

خارجي WidgetClass xmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
تعريفنوع بنية _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#جيڪڏهن_پورو
