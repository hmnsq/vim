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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EnhancedB_H
#ವ್ಯಾಖ್ಯಾನಿಸು EnhancedB_H

/*
 * New resources for the Extended Pushbutton widget
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmNshift
# define XmNshift		"shift"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmCShift
# define XmCShift		"Shift"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmNlabelLocation
# define XmNlabelLocation	"labelLocation"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmCLocation
# define XmCLocation		"Location"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ Xm_Npixmap_ದತ್ತಾಂಶ
# define Xm_Npixmap_ದತ್ತಾಂಶ		"pixmapData"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmNpixmapFile
# define XmNpixmapFile		"pixmapFile"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Constants for labelLocation.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_XM_JOINSIDET_H
# include <Xm/JoinSideT.h>
#ಇಲ್ಲದಿದ್ದರೆ
# define Xm_ಎಡ	    1
# define Xm_ಬಲ    2
# define Xm_ತುದಿ	    3
# define XmBOTTOM   4
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು XmIsEnhancedButton(w) XtIsSubclass(w, xmEnhancedButtonWidgetClass)

/*
 * Convenience creation function.
 */
ಬಾಹ್ಯ Widget XgCreateEPushButtonWidget(Widget, ಅಕ್ಷರ *, ArgList, Cardinal);

ಬಾಹ್ಯ WidgetClass xmEnhancedButtonWidgetClass;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _XmEnhancedButtonClassRec *XmEnhancedButtonWidgetClass;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _XmEnhancedButtonRec *XmEnhancedButtonWidget;

#ಆದರೆ_ಅಂತ್ಯ
