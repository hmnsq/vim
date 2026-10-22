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

#నిర్వచితం_కాకపోతే EnhancedB_H
#నిర్వచించు EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#నిర్వచితం_కాకపోతే XmNshift
# define XmNshift		"shift"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే XmCShift
# define XmCShift		"Shift"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే XmCLocation
# define XmCLocation		"Location"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే Xm_Npixmap_దత్తాంశం
# define Xm_Npixmap_దత్తాంశం		"pixmapData"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#అయితే_ముగింపు

/*
 * Constants for labelLocation.
 */
#నిర్వచితమైతే HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#లేకపోతే
# define Xm_ఎడమ	    1
# define Xm_కుడి    2
# define Xm_శిఖరం	    3
# define XmBOTTOM   4
#అయితే_ముగింపు

#నిర్వచించు XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
బాహ్య Widget XgCreateEPushButtonWidget(Widget, అక్షరం *, ArgList, Cardinal);

బాహ్య WidgetClass xmEnhancedButtonWidgetClass;
రకనిర్వచనం నిర్మాణం _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
రకనిర్వచనం నిర్మాణం _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#అయితే_ముగింపు
