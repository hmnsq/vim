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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EnhancedB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmNshift
# define XmNshift		"shift"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmCShift
# define XmCShift		"Shift"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmCLocation
# define XmCLocation		"Location"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ Xm_Npixmap_ਅੰਕੜੇ
# define Xm_Npixmap_ਅੰਕੜੇ		"pixmapData"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#ਜੇ_ਅੰਤ

/*
 * Constants for labelLocation.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#ਨਹੀਂਤਾਂ
# define Xm_ਖੱਬਾ	    1
# define Xm_ਸੱਜਾ    2
# define Xm_ਸਿਖਰ	    3
# define XmBOTTOM   4
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
ਬਾਹਰੀ Widget XgCreateEPushButtonWidget(Widget, ਅੱਖਰ *, ArgList, Cardinal);

ਬਾਹਰੀ WidgetClass xmEnhancedButtonWidgetClass;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#ਜੇ_ਅੰਤ
