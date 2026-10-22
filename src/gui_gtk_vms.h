/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * File MOTIF_REDEFINES.H originally delivers together with
 * OpenVMS Porting Library
 * defunct link: openvms.compaq.com/openvms/products/ips/porting.html
 *
 * This file has been modified for Vim development.
 * Original file contains just defines that GTK for OpenVMS uses,
 * but not all functions that DECW library has. Therefore it has been expanded
 * with necessary defines for Vim on OpenVMS with GTK GUI.
 *
 * Zoltan Arpadffy <arpadffy@polarhome.com>
 */

/*
 *************************************************************************
 *									 *
 * Copyright 2000 Compaq Computer Corporation				 *
 *									 *
 * COMPAQ Registered in U.S. Patent and Trademark Office.		 *
 *									 *
 *************************************************************************
 * IMPORTANT: Carefully read the License Terms below before		 *
 * proceeding.	By use of these materials you agree to these terms.	 *
 * If you do not agree to these terms, you may not use this software or  *
 * the accompanying documentation.					 *
 *************************************************************************
 * LICENSE TERMS							 *
 * 1. GRANT								 *
 * Compaq Computer Corporation ("COMPAQ") grants you the right to use,	 *
 * modify, and distribute the following source code (the "Software")	 *
 * on any number of computers. You may use the Software as part of	 *
 * creating a software program or product intended for commercial or	 *
 * non-commercial distribution in machine-readable source code, binary,  *
 * or executable formats. You may distribute the Software as		 *
 * machine-readable source code provided this license is not removed	 *
 * from the Software and any modifications are conspicuously indicated.  *
 * 2. COPYRIGHT								 *
 * The Software is owned by COMPAQ and its suppliers and is protected by *
 * copyright laws and international treaties.  Your use of the Software  *
 * and associated documentation is subject to the applicable copyright	 *
 * laws and the express rights and restrictions of these terms.		 *
 * 3. RESTRICTIONS							 *
 * You may not remove any copyright, trademark, or other proprietary	 *
 * notices from the Software or the associated	documentation.		 *
 * You are responsible for compliance with all applicable export or	 *
 * re-export control laws and regulations if you export the Software.	 *
 * This license is governed by and is to be construed under the laws	 *
 * of the State of Texas.						 *
 *									 *
 * DISCLAIMER OF WARRANTY AND LIABILITY					 *
 * Compaq shall not be liable for technical or editorial errors or	 *
 * omissions contained herein. The information contained herein is	 *
 * subject to change without notice.					 *
 *									 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.	 *
 * THE ENTIRE RISK ARISING OUT OF THE USE OF THIS SOFTWARE REMAINS WITH  *
 * RECIPIENT.  IN NO EVENT SHALL COMPAQ BE LIABLE FOR ANY DIRECT,	 *
 * CONSEQUENTIAL, INCIDENTAL, SPECIAL, PUNITIVE OR OTHER DAMAGES	 *
 * WHATSOEVER (INCLUDING WITHOUT LIMITATION DAMAGES FOR LOSS OF BUSINESS *
 * PROFITS, BUSINESS INTERRUPTION, OR LOSS OF BUSINESS INFORMATION),	 *
 * EVEN IF COMPAQ HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES	 *
 * AND WHETHER IN AN ACTION OF CONTRACT OR TORT INCLUDING NEGLIGENCE.	 *
 *									 *
 * If you have any questions concerning this license, please contact:	 *
 * Compaq Computer Corporation, Software Business Practices, ZKO1-2/D22, *
 * 110 Spit Brook Road, Nashua, NH. 03062-2698.				 *
 *									 *
 *************************************************************************
 */

// INTRINSIC.H omits proto if XtFree is defined
// VMS_BEGIN_C_PLUS_PLUS
ಬಾಹ್ಯ ಶೂನ್ಯ XtFree(ಅಕ್ಷರ*);
ಬಾಹ್ಯ ಶೂನ್ಯ XTFREE(ಅಕ್ಷರ*);
// VMS_END_C_PLUS_PLUS

#ವ್ಯಾಖ್ಯಾನಿಸು _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#ವ್ಯಾಖ್ಯಾನಿಸು XAllocClassHint XALLOCCLASSHINT
#ವ್ಯಾಖ್ಯಾನಿಸು XAllocColor XALLOCCOLOR
#ವ್ಯಾಖ್ಯಾನಿಸು XAllocColorCells XALLOCCOLORCELLS
#ವ್ಯಾಖ್ಯಾನಿಸು XAlloc_ಗಾತ್ರ_Hints XALLOCSIZEHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XAllocWMHints XALLOCWMHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XAutoRepeatOff XAUTOREPEATOFF
#ವ್ಯಾಖ್ಯಾನಿಸು XAutoRepeatOn XAUTOREPEATON
#ವ್ಯಾಖ್ಯಾನಿಸು XBase_Font_Name_List_ದ_Font_ಗಣ XBASEFONTNAMELISTOFFONTSET
#ವ್ಯಾಖ್ಯಾನಿಸು XBell XBELL
#ವ್ಯಾಖ್ಯಾನಿಸು XBitmapPad XBITMAPPAD
#ವ್ಯಾಖ್ಯಾನಿಸು XChange_ಸಕ್ರಿಯ_ಸೂಚಕ_Grab XCHANGEACTIVEPOINTERGRAB
#ವ್ಯಾಖ್ಯಾನಿಸು XChangeGC XCHANGEGC
#ವ್ಯಾಖ್ಯಾನಿಸು XChangeProperty XCHANGEPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು XChange_ಕಿಟಕಿ_ಗುಣಲಕ್ಷಣಗಳು XCHANGEWINDOWATTRIBUTES
#ವ್ಯಾಖ್ಯಾನಿಸು XCheckIfEvent XCHECKIFEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XCheck_ಮುಖವಾಡ_Event XCHECKMASKEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XCheckTypedEvent XCHECKTYPEDEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XCheck_Typed_ಕಿಟಕಿ_Event XCHECKTYPEDWINDOWEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XCheck_ಕಿಟಕಿ_Event XCHECKWINDOWEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XClearArea XCLEARAREA
#ವ್ಯಾಖ್ಯಾನಿಸು XClear_ಕಿಟಕಿ XCLEARWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XClipBox XCLIPBOX
#ವ್ಯಾಖ್ಯಾನಿಸು XCloseDisplay XCLOSEDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XCloseIM XCLOSEIM
#ವ್ಯಾಖ್ಯಾನಿಸು XConfigure_ಕಿಟಕಿ XCONFIGUREWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XConvertSelection XCONVERTSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು XCopyArea XCOPYAREA
#ವ್ಯಾಖ್ಯಾನಿಸು XCopyGC XCOPYGC
#ವ್ಯಾಖ್ಯಾನಿಸು XCopyPlane XCOPYPLANE
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_Bitmap_From_ದತ್ತಾಂಶ XCREATEBITMAPFROMDATA
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateColormap XCREATECOLORMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateFontCursor XCREATEFONTCURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_Font_ಗಣ XCREATEFONTSET
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateGC XCREATEGC
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateIC XCREATEIC
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateImage XCREATEIMAGE
#ವ್ಯಾಖ್ಯಾನಿಸು XCreatePixmap XCREATEPIXMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XCreatePixmapCursor XCREATEPIXMAPCURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_Pixmap_From_Bitmap_ದತ್ತಾಂಶ XCREATEPIXMAPFROMBITMAPDATA
#ವ್ಯಾಖ್ಯಾನಿಸು XCreateRegion XCREATEREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_Simple_ಕಿಟಕಿ XCREATESIMPLEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_ಕಿಟಕಿ XCREATEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XDefault_ಬೇರು_ಕಿಟಕಿ XDEFAULTROOTWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XDefault_Screen_ದ_Display XDEFAULTSCREENOFDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XDefineCursor XDEFINECURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು XDeleteProperty XDELETEPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು XDestroyIC XDESTROYIC
#ವ್ಯಾಖ್ಯಾನಿಸು XDestroyRegion XDESTROYREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XDestroy_ಕಿಟಕಿ XDESTROYWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XDisplayName XDISPLAYNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XDisplay_ದ_Screen XDISPLAYOFSCREEN
#ವ್ಯಾಖ್ಯಾನಿಸು XDisplayString XDISPLAYSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawArc XDRAWARC
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawImageString XDRAWIMAGESTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawImageString16 XDRAWIMAGESTRING16
#ವ್ಯಾಖ್ಯಾನಿಸು XDraw_ಸಾಲು XDRAWLINE
#ವ್ಯಾಖ್ಯಾನಿಸು XDraw_ಸಾಲುಗಳು XDRAWLINES
#ವ್ಯಾಖ್ಯಾನಿಸು XDraw_ಬಿಂದು XDRAWPOINT
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawPoints XDRAWPOINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawRectangle XDRAWRECTANGLE
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawSegments XDRAWSEGMENTS
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawString XDRAWSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XDrawString16 XDRAWSTRING16
#ವ್ಯಾಖ್ಯಾನಿಸು XEmptyRegion XEMPTYREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XEqualRegion XEQUALREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XEventsQueued XEVENTSQUEUED
#ವ್ಯಾಖ್ಯಾನಿಸು XExtents_ದ_Font_ಗಣ XEXTENTSOFFONTSET
#ವ್ಯಾಖ್ಯಾನಿಸು XFetch_ಬಫರ್ XFETCHBUFFER
#ವ್ಯಾಖ್ಯಾನಿಸು XFillArc XFILLARC
#ವ್ಯಾಖ್ಯಾನಿಸು XFillPolygon XFILLPOLYGON
#ವ್ಯಾಖ್ಯಾನಿಸು XFillRectangle XFILLRECTANGLE
#ವ್ಯಾಖ್ಯಾನಿಸು XFillRectangles XFILLRECTANGLES
#ವ್ಯಾಖ್ಯಾನಿಸು XFilterEvent XFILTEREVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XFlush XFLUSH
#ವ್ಯಾಖ್ಯಾನಿಸು XFonts_ದ_Font_ಗಣ XFONTSOFFONTSET
#ವ್ಯಾಖ್ಯಾನಿಸು XFree XFREE
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeColormap XFREECOLORMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeColors XFREECOLORS
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeCursor XFREECURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeFont XFREEFONT
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeFontInfo XFREEFONTINFO
#ವ್ಯಾಖ್ಯಾನಿಸು XFree_Font_ಹೆಸರುಗಳು XFREEFONTNAMES
#ವ್ಯಾಖ್ಯಾನಿಸು XFree_Font_ಗಣ XFREEFONTSET
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeGC XFREEGC
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeModifiermap XFREEMODIFIERMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XFreePixmap XFREEPIXMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XFreeStringList XFREESTRINGLIST
#ವ್ಯಾಖ್ಯಾನಿಸು XGetAtomName XGETATOMNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XGetDefault XGETDEFAULT
#ವ್ಯಾಖ್ಯಾನಿಸು XGet_Error_Database_ಪಠ್ಯ XGETERRORDATABASETEXT
#ವ್ಯಾಖ್ಯಾನಿಸು XGet_Error_ಪಠ್ಯ XGETERRORTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು XGetFontProperty XGETFONTPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು XGetGCValues XGETGCVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XGetGeometry XGETGEOMETRY
#ವ್ಯಾಖ್ಯಾನಿಸು XGetIconSizes XGETICONSIZES
#ವ್ಯಾಖ್ಯಾನಿಸು XGetICValues XGETICVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XGetIMValues XGETIMVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XGetImage XGETIMAGE
#ವ್ಯಾಖ್ಯಾನಿಸು XGetKeyboardControl XGETKEYBOARDCONTROL
#ವ್ಯಾಖ್ಯಾನಿಸು XGetModifierMapping XGETMODIFIERMAPPING
#ವ್ಯಾಖ್ಯಾನಿಸು XGetMotionEvents XGETMOTIONEVENTS
#ವ್ಯಾಖ್ಯಾನಿಸು XGet_ಸಾಮಾನ್ಯ_Hints XGETNORMALHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XGetSelectionOwner XGETSELECTIONOWNER
#ವ್ಯಾಖ್ಯಾನಿಸು XGetSubImage XGETSUBIMAGE
#ವ್ಯಾಖ್ಯಾನಿಸು XGetVisualInfo XGETVISUALINFO
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMIconName XGETWMICONNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMProtocols XGETWMPROTOCOLS
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMHints XGETWMHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMName XGETWMNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XGetWMNormalHints XGETWMNORMALHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XGet_ಕಿಟಕಿ_ಗುಣಲಕ್ಷಣಗಳು XGETWINDOWATTRIBUTES
#ವ್ಯಾಖ್ಯಾನಿಸು XGet_ಕಿಟಕಿ_Property XGETWINDOWPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು XGrabKeyboard XGRABKEYBOARD
#ವ್ಯಾಖ್ಯಾನಿಸು XGrab_ಸೂಚಕ XGRABPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು XGrabServer XGRABSERVER
#ವ್ಯಾಖ್ಯಾನಿಸು XHeight_ದ_Screen XHEIGHTOFSCREEN
#ವ್ಯಾಖ್ಯಾನಿಸು XIconify_ಕಿಟಕಿ XICONIFYWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XIfEvent XIFEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XInternAtom XINTERNATOM
#ವ್ಯಾಖ್ಯಾನಿಸು XIntersectRegion XINTERSECTREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XKeycode_ಗಮ್ಯ_Keysym XKEYCODETOKEYSYM
#ವ್ಯಾಖ್ಯಾನಿಸು XKeysym_ಗಮ್ಯ_Keycode XKEYSYMTOKEYCODE
#ವ್ಯಾಖ್ಯಾನಿಸು XKeysym_ಗಮ್ಯ_String XKEYSYMTOSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XListFonts XLISTFONTS
#ವ್ಯಾಖ್ಯಾನಿಸು XListFontsWithInfo XLISTFONTSWITHINFO
#ವ್ಯಾಖ್ಯಾನಿಸು XListPixmapFormats XLISTPIXMAPFORMATS
#ವ್ಯಾಖ್ಯಾನಿಸು XListProperties XLISTPROPERTIES
#ವ್ಯಾಖ್ಯಾನಿಸು XLoadQueryFont XLOADQUERYFONT
#ವ್ಯಾಖ್ಯಾನಿಸು XLookupString XLOOKUPSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XLower_ಕಿಟಕಿ XLOWERWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XMapRaised XMAPRAISED
#ವ್ಯಾಖ್ಯಾನಿಸು XMap_ಕಿಟಕಿ XMAPWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XMatchVisualInfo XMATCHVISUALINFO
#ವ್ಯಾಖ್ಯಾನಿಸು XMove_Resize_ಕಿಟಕಿ XMOVERESIZEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XMove_ಕಿಟಕಿ XMOVEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XNextEvent XNEXTEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XOffsetRegion XOFFSETREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XOpenDisplay XOPENDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XOpenIM XOPENIM
#ವ್ಯಾಖ್ಯಾನಿಸು XParseColor XPARSECOLOR
#ವ್ಯಾಖ್ಯಾನಿಸು XParseGeometry XPARSEGEOMETRY
#ವ್ಯಾಖ್ಯಾನಿಸು XPeekEvent XPEEKEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XPending XPENDING
#ವ್ಯಾಖ್ಯಾನಿಸು XPoint_ಒಳ_Region XPOINTINREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XPolygonRegion XPOLYGONREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XPut_ಹಿಂಭಾಗ_Event XPUTBACKEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XPutImage XPUTIMAGE
#ವ್ಯಾಖ್ಯಾನಿಸು XRoot_ಕಿಟಕಿ XROOTWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XQueryColor XQUERYCOLOR
#ವ್ಯಾಖ್ಯಾನಿಸು XQueryColors XQUERYCOLORS
#ವ್ಯಾಖ್ಯಾನಿಸು XQueryExtension XQUERYEXTENSION
#ವ್ಯಾಖ್ಯಾನಿಸು XQuery_ಸೂಚಕ XQUERYPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು XQuery_ವೃಕ್ಷ XQUERYTREE
#ವ್ಯಾಖ್ಯಾನಿಸು XRaise_ಕಿಟಕಿ XRAISEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XReconfigureWMWindow XRECONFIGUREWMWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XRect_ಒಳ_Region XRECTINREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#ವ್ಯಾಖ್ಯಾನಿಸು XReparent_ಕಿಟಕಿ XREPARENTWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XResize_ಕಿಟಕಿ XRESIZEWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XRestackWindows XRESTACKWINDOWS
#ವ್ಯಾಖ್ಯಾನಿಸು XRoot_ಕಿಟಕಿ_ದ_Screen XROOTWINDOWOFSCREEN
#ವ್ಯಾಖ್ಯಾನಿಸು XScreen_ಸಂಖ್ಯೆ_ದ_Screen XSCREENNUMBEROFSCREEN
#ವ್ಯಾಖ್ಯಾನಿಸು XSelectAsyncEvent XSELECTASYNCEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XSelect_Async_ಒಳಹರಿವು XSELECTASYNCINPUT
#ವ್ಯಾಖ್ಯಾನಿಸು XSelect_ಒಳಹರಿವು XSELECTINPUT
#ವ್ಯಾಖ್ಯಾನಿಸು XSendEvent XSENDEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XServerVendor XSERVERVENDOR
#ವ್ಯಾಖ್ಯಾನಿಸು XSetBackground XSETBACKGROUND
#ವ್ಯಾಖ್ಯಾನಿಸು XSetClassHint XSETCLASSHINT
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_Clip_ಮುಖವಾಡ XSETCLIPMASK
#ವ್ಯಾಖ್ಯಾನಿಸು XSetClipOrigin XSETCLIPORIGIN
#ವ್ಯಾಖ್ಯಾನಿಸು XSetClipRectangles XSETCLIPRECTANGLES
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_Close_ಕೆಳಕ್ಕೆ_ಬಹುಲಕ XSETCLOSEDOWNMODE
#ವ್ಯಾಖ್ಯಾನಿಸು XSetCommand XSETCOMMAND
#ವ್ಯಾಖ್ಯಾನಿಸು XSetDashes XSETDASHES
#ವ್ಯಾಖ್ಯಾನಿಸು XSetErrorHandler XSETERRORHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು XSetFillStyle XSETFILLSTYLE
#ವ್ಯಾಖ್ಯಾನಿಸು XSetFont XSETFONT
#ವ್ಯಾಖ್ಯಾನಿಸು XSetForeground XSETFOREGROUND
#ವ್ಯಾಖ್ಯಾನಿಸು XSetFunction XSETFUNCTION
#ವ್ಯಾಖ್ಯಾನಿಸು XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#ವ್ಯಾಖ್ಯಾನಿಸು XSetICFocus XSETICFOCUS
#ವ್ಯಾಖ್ಯಾನಿಸು XSetICValues XSETICVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XSetIOErrorHandler XSETIOERRORHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಒಳಹರಿವು_Focus XSETINPUTFOCUS
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಸಾಲು_ಗುಣಲಕ್ಷಣಗಳು XSETLINEATTRIBUTES
#ವ್ಯಾಖ್ಯಾನಿಸು XSetLocaleModifiers XSETLOCALEMODIFIERS
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಸಾಮಾನ್ಯ_Hints XSETNORMALHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XSetRegion XSETREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XSetSelectionOwner XSETSELECTIONOWNER
#ವ್ಯಾಖ್ಯಾನಿಸು XSetStipple XSETSTIPPLE
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_Subwindow_ಬಹುಲಕ XSETSUBWINDOWMODE
#ವ್ಯಾಖ್ಯಾನಿಸು XSetTSOrigin XSETTSORIGIN
#ವ್ಯಾಖ್ಯಾನಿಸು XSetTile XSETTILE
#ವ್ಯಾಖ್ಯಾನಿಸು XSetTransientForHint XSETTRANSIENTFORHINT
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMHints XSETWMHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMIconName XSETWMICONNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMName XSETWMNAME
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMNormalHints XSETWMNORMALHINTS
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMProperties XSETWMPROPERTIES
#ವ್ಯಾಖ್ಯಾನಿಸು XSetWMProtocols XSETWMPROTOCOLS
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಕಿಟಕಿ_Background XSETWINDOWBACKGROUND
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಕಿಟಕಿ_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XSet_ಕಿಟಕಿ_Colormap XSETWINDOWCOLORMAP
#ವ್ಯಾಖ್ಯಾನಿಸು XShape_Combine_ಮುಖವಾಡ XSHAPECOMBINEMASK
#ವ್ಯಾಖ್ಯಾನಿಸು XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#ವ್ಯಾಖ್ಯಾನಿಸು XShapeGetRectangles XSHAPEGETRECTANGLES
#ವ್ಯಾಖ್ಯಾನಿಸು XShrinkRegion XSHRINKREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XStore_ಬಫರ್ XSTOREBUFFER
#ವ್ಯಾಖ್ಯಾನಿಸು XStoreColor XSTORECOLOR
#ವ್ಯಾಖ್ಯಾನಿಸು XStoreColors XSTORECOLORS
#ವ್ಯಾಖ್ಯಾನಿಸು XStoreName XSTORENAME
#ವ್ಯಾಖ್ಯಾನಿಸು XString_ಗಮ್ಯ_Keysym XSTRINGTOKEYSYM
#ವ್ಯಾಖ್ಯಾನಿಸು XSubtractRegion XSUBTRACTREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XSupportsLocale XSUPPORTSLOCALE
#ವ್ಯಾಖ್ಯಾನಿಸು XSync XSYNC
#ವ್ಯಾಖ್ಯಾನಿಸು XSynchronize XSYNCHRONIZE
#ವ್ಯಾಖ್ಯಾನಿಸು XTextExtents XTEXTEXTENTS
#ವ್ಯಾಖ್ಯಾನಿಸು XTextExtents16 XTEXTEXTENTS16
#ವ್ಯಾಖ್ಯಾನಿಸು XText_ಅಗಲ XTEXTWIDTH
#ವ್ಯಾಖ್ಯಾನಿಸು XTextWidth16 XTEXTWIDTH16
#ವ್ಯಾಖ್ಯಾನಿಸು XTranslateCoordinates XTRANSLATECOORDINATES
#ವ್ಯಾಖ್ಯಾನಿಸು XUndefineCursor XUNDEFINECURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು XUngrabKeyboard XUNGRABKEYBOARD
#ವ್ಯಾಖ್ಯಾನಿಸು XUngrab_ಸೂಚಕ XUNGRABPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು XUngrabServer XUNGRABSERVER
#ವ್ಯಾಖ್ಯಾನಿಸು XUnionRectWithRegion XUNIONRECTWITHREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XUnionRegion XUNIONREGION
#ವ್ಯಾಖ್ಯಾನಿಸು XUnmap_ಕಿಟಕಿ XUNMAPWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು _XUnregisterFilter _XUNREGISTERFILTER
#ವ್ಯಾಖ್ಯಾನಿಸು XUnsetICFocus XUNSETICFOCUS
#ವ್ಯಾಖ್ಯಾನಿಸು XVaCreateNestedList XVACREATENESTEDLIST
#ವ್ಯಾಖ್ಯಾನಿಸು XVisualIDFromVisual XVISUALIDFROMVISUAL
#ವ್ಯಾಖ್ಯಾನಿಸು XWarp_ಸೂಚಕ XWARPPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು XWidth_ದ_Screen XWIDTHOFSCREEN
#ವ್ಯಾಖ್ಯಾನಿಸು XWindowEvent XWINDOWEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XWithdraw_ಕಿಟಕಿ XWITHDRAWWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XXorRegion XXORREGION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಕೂಡಿಸು_Protocol_Callback XMADDPROTOCOLCALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಕೂಡಿಸು_Protocols XMADDPROTOCOLS
#ವ್ಯಾಖ್ಯಾನಿಸು XmChangeColor XMCHANGECOLOR
#ವ್ಯಾಖ್ಯಾನಿಸು XmClipboardCopy XMCLIPBOARDCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Clipboard_ಕೊನೆ_Copy XMCLIPBOARDENDCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Clipboard_Inquire_ಉದ್ದಳತೆ XMCLIPBOARDINQUIRELENGTH
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Clipboard_ಬೀಗ XMCLIPBOARDLOCK
#ವ್ಯಾಖ್ಯಾನಿಸು XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Clipboard_ಆರಂಭ_Copy XMCLIPBOARDSTARTCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು XmClipboardUnlock XMCLIPBOARDUNLOCK
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateArrowButton XMCREATEARROWBUTTON
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateCascadeButton XMCREATECASCADEBUTTON
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateDialogShell XMCREATEDIALOGSHELL
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateDragIcon XMCREATEDRAGICON
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateDrawingArea XMCREATEDRAWINGAREA
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateDrawnButton XMCREATEDRAWNBUTTON
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateForm XMCREATEFORM
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateFormDialog XMCREATEFORMDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಚೌಕಟ್ಟು XMCREATEFRAME
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateLabelGadget XMCREATELABELGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_Main_ಕಿಟಕಿ XMCREATEMAINWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateMenuBar XMCREATEMENUBAR
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಸಂದೇಶ_Box XMCREATEMESSAGEBOX
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಸಂದೇಶ_Dialog XMCREATEMESSAGEDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateOptionMenu XMCREATEOPTIONMENU
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_Paned_ಕಿಟಕಿ XMCREATEPANEDWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreatePopupMenu XMCREATEPOPUPMENU
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreatePromptDialog XMCREATEPROMPTDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ತಳ್ಳು_Button XMCREATEPUSHBUTTON
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ತಳ್ಳು_Button_Gadget XMCREATEPUSHBUTTONGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateRadioBox XMCREATERADIOBOX
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ XMCREATEROWCOLUMN
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateScale XMCREATESCALE
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateScrollBar XMCREATESCROLLBAR
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateScrolledList XMCREATESCROLLEDLIST
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_Scrolled_ಪಠ್ಯ XMCREATESCROLLEDTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_Scrolled_ಕಿಟಕಿ XMCREATESCROLLEDWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateSeparator XMCREATESEPARATOR
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಪಠ್ಯ XMCREATETEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Create_ಪಠ್ಯ_ಕ್ಷೇತ್ರ XMCREATETEXTFIELD
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateToggleButton XMCREATETOGGLEBUTTON
#ವ್ಯಾಖ್ಯಾನಿಸು XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Drag_ಆರಂಭ XMDRAGSTART
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Drop_Site_ರಿಜಿಸ್ಟರ್ XMDROPSITEREGISTER
#ವ್ಯಾಖ್ಯಾನಿಸು XmDropSiteUnregister XMDROPSITEUNREGISTER
#ವ್ಯಾಖ್ಯಾನಿಸು XmDropSiteUpdate XMDROPSITEUPDATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Drop_Transfer_ಆರಂಭ XMDROPTRANSFERSTART
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_File_Selection_Box_Get_ಮಗು XMFILESELECTIONBOXGETCHILD
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_File_Selection_Do_ಹುಡುಕು XMFILESELECTIONDOSEARCH
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_Append_ನಮೂದು XMFONTLISTAPPENDENTRY
#ವ್ಯಾಖ್ಯಾನಿಸು XmFontListCopy XMFONTLISTCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು XmFontListCreate XMFONTLISTCREATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ನಮೂದು_Create XMFONTLISTENTRYCREATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ನಮೂದು_Free XMFONTLISTENTRYFREE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ನಮೂದು_Get_Font XMFONTLISTENTRYGETFONT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ನಮೂದು_Get_ಟ್ಯಾಗ್ XMFONTLISTENTRYGETTAG
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ನಮೂದು_ತುಂಬು XMFONTLISTENTRYLOAD
#ವ್ಯಾಖ್ಯಾನಿಸು XmFontListFree XMFONTLISTFREE
#ವ್ಯಾಖ್ಯಾನಿಸು XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_Get_ಮುಂದಿನ_Font XMFONTLISTGETNEXTFONT
#ವ್ಯಾಖ್ಯಾನಿಸು XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Font_List_ಮುಂದಿನ_ನಮೂದು XMFONTLISTNEXTENTRY
#ವ್ಯಾಖ್ಯಾನಿಸು XmGetColors XMGETCOLORS
#ವ್ಯಾಖ್ಯಾನಿಸು XmGetFocusWidget XMGETFOCUSWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmGetMenuCursor XMGETMENUCURSOR
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Get_Pixmap_By_ಆಳ XMGETPIXMAPBYDEPTH
#ವ್ಯಾಖ್ಯಾನಿಸು XmGetTearOffControl XMGETTEAROFFCONTROL
#ವ್ಯಾಖ್ಯಾನಿಸು XmGetXmDisplay XMGETXMDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XmImMbLookupString XMIMMBLOOKUPSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Im_ರಿಜಿಸ್ಟರ್ XMIMREGISTER
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Im_ಗಣ_Focus_ಮೌಲ್ಯಗಳು XMIMSETFOCUSVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Im_ಗಣ_ಮೌಲ್ಯಗಳು XMIMSETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XmImUnregister XMIMUNREGISTER
#ವ್ಯಾಖ್ಯಾನಿಸು XmImUnsetFocus XMIMUNSETFOCUS
#ವ್ಯಾಖ್ಯಾನಿಸು XmInternAtom XMINTERNATOM
#ವ್ಯಾಖ್ಯಾನಿಸು XmIsMotifWMRunning XMISMOTIFWMRUNNING
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಕೂಡಿಸು_ಅಂಶ XMLISTADDITEM
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಕೂಡಿಸು_ಅಂಶ_Unselected XMLISTADDITEMUNSELECTED
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಕೂಡಿಸು_ವಸ್ತುಗಳು_Unselected XMLISTADDITEMSUNSELECTED
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Delete_All_ವಸ್ತುಗಳು XMLISTDELETEALLITEMS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Delete_ವಸ್ತುಗಳು_Pos XMLISTDELETEITEMSPOS
#ವ್ಯಾಖ್ಯಾನಿಸು XmListDeletePos XMLISTDELETEPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Deselect_All_ವಸ್ತುಗಳು XMLISTDESELECTALLITEMS
#ವ್ಯಾಖ್ಯಾನಿಸು XmListDeselectPos XMLISTDESELECTPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Get_Kbd_ಅಂಶ_Pos XMLISTGETKBDITEMPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Get_ಹೊಂದಿಕೆ_Pos XMLISTGETMATCHPOS
#ವ್ಯಾಖ್ಯಾನಿಸು XmListGetSelectedPos XMLISTGETSELECTEDPOS
#ವ್ಯಾಖ್ಯಾನಿಸು XmListPosSelected XMLISTPOSSELECTED
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_Select_ಅಂಶ XMLISTSELECTITEM
#ವ್ಯಾಖ್ಯಾನಿಸು XmListSelectPos XMLISTSELECTPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಗಣ_Bottom_Pos XMLISTSETBOTTOMPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಗಣ_ಅಂಶ XMLISTSETITEM
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಗಣ_Kbd_ಅಂಶ_Pos XMLISTSETKBDITEMPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_List_ಗಣ_Pos XMLISTSETPOS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Main_ಕಿಟಕಿ_ಗಣ_Areas XMMAINWINDOWSETAREAS
#ವ್ಯಾಖ್ಯಾನಿಸು XmMenuPosition XMMENUPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಸಂದೇಶ_Box_Get_ಮಗು XMMESSAGEBOXGETCHILD
#ವ್ಯಾಖ್ಯಾನಿಸು XmOptionButtonGadget XMOPTIONBUTTONGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmOptionLabelGadget XMOPTIONLABELGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು XmProcessTraversal XMPROCESSTRAVERSAL
#ವ್ಯಾಖ್ಯಾನಿಸು XmQmotif XMQMOTIF
#ವ್ಯಾಖ್ಯಾನಿಸು XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು XmRepTypeGetId XMREPTYPEGETID
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Rep_Type_Get_ದಾಖಲೆ XMREPTYPEGETRECORD
#ವ್ಯಾಖ್ಯಾನಿಸು XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Rep_Type_ರಿಜಿಸ್ಟರ್ XMREPTYPEREGISTER
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Rep_Type_Valid_ಮೌಲ್ಯ XMREPTYPEVALIDVALUE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Scroll_Bar_ಗಣ_ಮೌಲ್ಯಗಳು XMSCROLLBARSETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Scrolled_ಕಿಟಕಿ_ಗಣ_Areas XMSCROLLEDWINDOWSETAREAS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Selection_Box_Get_ಮಗು XMSELECTIONBOXGETCHILD
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringByteCompare XMSTRINGBYTECOMPARE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCompare XMSTRINGCOMPARE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringConcat XMSTRINGCONCAT
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCopy XMSTRINGCOPY
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCreate XMSTRINGCREATE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCreateLtoR XMSTRINGCREATELTOR
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringCreateSimple XMSTRINGCREATESIMPLE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringDraw XMSTRINGDRAW
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringExtent XMSTRINGEXTENT
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringFree XMSTRINGFREE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringFreeContext XMSTRINGFREECONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringGetLtoR XMSTRINGGETLTOR
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_String_Get_ಮುಂದಿನ_Component XMSTRINGGETNEXTCOMPONENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_String_Get_ಮುಂದಿನ_Segment XMSTRINGGETNEXTSEGMENT
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringInitContext XMSTRINGINITCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_String_ಉದ್ದಳತೆ XMSTRINGLENGTH
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringLtoRCreate XMSTRINGLTORCREATE
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringNConcat XMSTRINGNCONCAT
#ವ್ಯಾಖ್ಯಾನಿಸು XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_String_ಅಗಲ XMSTRINGWIDTH
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Clear_Selection XMTEXTCLEARSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_Editable XMTEXTFIELDGETEDITABLE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_ಕೊನೆಯ_Position XMTEXTFIELDGETLASTPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_Selection XMTEXTFIELDGETSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_String XMTEXTFIELDGETSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಸೇರಿಸು XMTEXTFIELDINSERT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Remove XMTEXTFIELDREMOVE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_Selection XMTEXTFIELDSETSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_String XMTEXTFIELDSETSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_ಕೊನೆಯ_Position XMTEXTGETLASTPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_Max_ಉದ್ದಳತೆ XMTEXTGETMAXLENGTH
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_Selection XMTEXTGETSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Get_String XMTEXTGETSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಸೇರಿಸು XMTEXTINSERT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Remove XMTEXTREMOVE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_Replace XMTEXTREPLACE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಗಣ_Cursor_Position XMTEXTSETCURSORPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಗಣ_Highlight XMTEXTSETHIGHLIGHT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಗಣ_Insertion_Position XMTEXTSETINSERTIONPOSITION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಗಣ_Selection XMTEXTSETSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಪಠ್ಯ_ಗಣ_String XMTEXTSETSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Toggle_Button_Gadget_ಗಣ_State XMTOGGLEBUTTONGADGETSETSTATE
#ವ್ಯಾಖ್ಯಾನಿಸು XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Toggle_Button_ಗಣ_State XMTOGGLEBUTTONSETSTATE
#ವ್ಯಾಖ್ಯಾನಿಸು XmUpdateDisplay XMUPDATEDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#ವ್ಯಾಖ್ಯಾನಿಸು XmbDrawString XMBDRAWSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XmbLookupString XMBLOOKUPSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XmbResetIC XMBRESETIC
#ವ್ಯಾಖ್ಯಾನಿಸು Xmb_ಗಣ_WMProperties XMBSETWMPROPERTIES
#ವ್ಯಾಖ್ಯಾನಿಸು Xmb_ಪಠ್ಯ_Escapement XMBTEXTESCAPEMENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xmb_ಪಠ್ಯ_Extents XMBTEXTEXTENTS
#ವ್ಯಾಖ್ಯಾನಿಸು Xmb_ಪಠ್ಯ_List_ಗಮ್ಯ_ಪಠ್ಯ_Property XMBTEXTLISTTOTEXTPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು Xmb_ಪಠ್ಯ_Property_ಗಮ್ಯ_ಪಠ್ಯ_List XMBTEXTPROPERTYTOTEXTLIST
#ವ್ಯಾಖ್ಯಾನಿಸು Xmu_Client_ಕಿಟಕಿ XMUCLIENTWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು Xmu_Print_Default_Error_ಸಂದೇಶ XMUPRINTDEFAULTERRORMESSAGE
#ವ್ಯಾಖ್ಯಾನಿಸು XrmGetDatabase XRMGETDATABASE
#ವ್ಯಾಖ್ಯಾನಿಸು XrmGetResource XRMGETRESOURCE
#ವ್ಯಾಖ್ಯಾನಿಸು XrmPutStringResource XRMPUTSTRINGRESOURCE
#ವ್ಯಾಖ್ಯಾನಿಸು Xrm_Quark_ಗಮ್ಯ_String XRMQUARKTOSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು Xrm_String_ಗಮ್ಯ_Quark XRMSTRINGTOQUARK
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕೂಡಿಸು_Callback XTADDCALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕೂಡಿಸು_Callbacks XTADDCALLBACKS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕೂಡಿಸು_Event_Handler XTADDEVENTHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕೂಡಿಸು_Grab XTADDGRAB
#ವ್ಯಾಖ್ಯಾನಿಸು XtAllocateGC XTALLOCATEGC
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಕೂಡಿಸು_Actions XTAPPADDACTIONS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಕೂಡಿಸು_ಒಳಹರಿವು XTAPPADDINPUT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು XTAPPADDTIMEOUT
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppCreateShell XTAPPCREATESHELL
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppInitialize XTAPPINITIALIZE
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಮುಂದಿನ_Event XTAPPNEXTEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppPeekEvent XTAPPPEEKEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppPending XTAPPPENDING
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppProcessEvent XTAPPPROCESSEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಗಣ_Error_Handler XTAPPSETERRORHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಗಣ_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಗಣ_Warning_Handler XTAPPSETWARNINGHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_App_ಗಣ_Warning_ಸಂದೇಶ_Handler XTAPPSETWARNINGMSGHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು XtAppWarning XTAPPWARNING
#ವ್ಯಾಖ್ಯಾನಿಸು XtCallActionProc XTCALLACTIONPROC
#ವ್ಯಾಖ್ಯಾನಿಸು XtCallCallbackList XTCALLCALLBACKLIST
#ವ್ಯಾಖ್ಯಾನಿಸು XtCallCallbacks XTCALLCALLBACKS
#ವ್ಯಾಖ್ಯಾನಿಸು XtConfigureWidget XTCONFIGUREWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtConvertAndStore XTCONVERTANDSTORE
#ವ್ಯಾಖ್ಯಾನಿಸು XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtCreatePopupShell XTCREATEPOPUPSHELL
#ವ್ಯಾಖ್ಯಾನಿಸು XtCreateWidget XTCREATEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtDatabase XTDATABASE
#ವ್ಯಾಖ್ಯಾನಿಸು XtDestroyWidget XTDESTROYWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtDisownSelection XTDISOWNSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು XtDispatchEvent XTDISPATCHEVENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Display_ದ_Object XTDISPLAYOFOBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Display_ಗಮ್ಯ_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು XtFree XTFREE
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetActionKeysym XTGETACTIONKEYSYM
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetActionList XTGETACTIONLIST
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetGC XTGETGC
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Get_Multi_Click_ಸಮಯ XTGETMULTICLICKTIME
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Get_Selection_ಮೌಲ್ಯ XTGETSELECTIONVALUE
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Get_Selection_ಮೌಲ್ಯಗಳು XTGETSELECTIONVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XtGetSubresources XTGETSUBRESOURCES
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Get_ಮೌಲ್ಯಗಳು XTGETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XtGrabKeyboard XTGRABKEYBOARD
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Grab_ಸೂಚಕ XTGRABPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು XtHasCallbacks XTHASCALLBACKS
#ವ್ಯಾಖ್ಯಾನಿಸು XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಸೇರಿಸು_Event_Handler XTINSERTEVENTHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು XtIsManaged XTISMANAGED
#ವ್ಯಾಖ್ಯಾನಿಸು XtIsObject XTISOBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು XtIsSensitive XTISSENSITIVE
#ವ್ಯಾಖ್ಯಾನಿಸು XtIsSubclass XTISSUBCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕೊನೆಯ_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#ವ್ಯಾಖ್ಯಾನಿಸು XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#ವ್ಯಾಖ್ಯಾನಿಸು XtMakeResizeRequest XTMAKERESIZEREQUEST
#ವ್ಯಾಖ್ಯಾನಿಸು XtMalloc XTMALLOC
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Manage_ಮಗು XTMANAGECHILD
#ವ್ಯಾಖ್ಯಾನಿಸು XtManageChildren XTMANAGECHILDREN
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ವಿಲೀನ_Arg_Lists XTMERGEARGLISTS
#ವ್ಯಾಖ್ಯಾನಿಸು XtMoveWidget XTMOVEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtName XTNAME
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Name_ಗಮ್ಯ_Widget XTNAMETOWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtOpenDisplay XTOPENDISPLAY
#ವ್ಯಾಖ್ಯಾನಿಸು XtOverrideTranslations XTOVERRIDETRANSLATIONS
#ವ್ಯಾಖ್ಯಾನಿಸು XtOwnSelection XTOWNSELECTION
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಪೋಷಕ XTPARENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Parse_Translation_ಕೋಷ್ಟಕ XTPARSETRANSLATIONTABLE
#ವ್ಯಾಖ್ಯಾನಿಸು XtPopdown XTPOPDOWN
#ವ್ಯಾಖ್ಯಾನಿಸು XtPopup XTPOPUP
#ವ್ಯಾಖ್ಯಾನಿಸು XtQueryGeometry XTQUERYGEOMETRY
#ವ್ಯಾಖ್ಯಾನಿಸು XtRealizeWidget XTREALIZEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtRealloc XTREALLOC
#ವ್ಯಾಖ್ಯಾನಿಸು XtReleaseGC XTRELEASEGC
#ವ್ಯಾಖ್ಯಾನಿಸು XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#ವ್ಯಾಖ್ಯಾನಿಸು XtRemoveCallback XTREMOVECALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು XtRemoveEventHandler XTREMOVEEVENTHANDLER
#ವ್ಯಾಖ್ಯಾನಿಸು XtRemoveGrab XTREMOVEGRAB
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Remove_ಒಳಹರಿವು XTREMOVEINPUT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು XTREMOVETIMEOUT
#ವ್ಯಾಖ್ಯಾನಿಸು XtResizeWidget XTRESIZEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtResolvePathname XTRESOLVEPATHNAME
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಗಣ_Keyboard_Focus XTSETKEYBOARDFOCUS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಗಣ_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಗಣ_Sensitive XTSETSENSITIVE
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಗಣ_Type_Converter XTSETTYPECONVERTER
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಗಣ_ಮೌಲ್ಯಗಳು XTSETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Shell_ಅಕ್ಷರಸರಣಿಗಳು XTSHELLSTRINGS
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಅಕ್ಷರಸರಣಿಗಳು XTSTRINGS
#ವ್ಯಾಖ್ಯಾನಿಸು XtToolkitInitialize XTTOOLKITINITIALIZE
#ವ್ಯಾಖ್ಯಾನಿಸು XtTranslateCoords XTTRANSLATECOORDS
#ವ್ಯಾಖ್ಯಾನಿಸು XtTranslateKeycode XTTRANSLATEKEYCODE
#ವ್ಯಾಖ್ಯಾನಿಸು XtUngrabKeyboard XTUNGRABKEYBOARD
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Ungrab_ಸೂಚಕ XTUNGRABPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Unmanage_ಮಗು XTUNMANAGECHILD
#ವ್ಯಾಖ್ಯಾನಿಸು XtUnmanageChildren XTUNMANAGECHILDREN
#ವ್ಯಾಖ್ಯಾನಿಸು XtUnrealizeWidget XTUNREALIZEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtVaAppCreateShell XTVAAPPCREATESHELL
#ವ್ಯಾಖ್ಯಾನಿಸು XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#ವ್ಯಾಖ್ಯಾನಿಸು XtVaCreateWidget XTVACREATEWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Va_Get_ಮೌಲ್ಯಗಳು XTVAGETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು XTVASETVALUES
#ವ್ಯಾಖ್ಯಾನಿಸು XtWarning XTWARNING
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Widget_ಗಮ್ಯ_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕಿಟಕಿ XTWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕಿಟಕಿ_ದ_Object XTWINDOWOFOBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ಕಿಟಕಿ_ಗಮ್ಯ_Widget XTWINDOWTOWIDGET
#ವ್ಯಾಖ್ಯಾನಿಸು XwcDrawString XWCDRAWSTRING
#ವ್ಯಾಖ್ಯಾನಿಸು XwcFreeStringList XWCFREESTRINGLIST
#ವ್ಯಾಖ್ಯಾನಿಸು Xwc_ಪಠ್ಯ_Escapement XWCTEXTESCAPEMENT
#ವ್ಯಾಖ್ಯಾನಿಸು Xwc_ಪಠ್ಯ_Extents XWCTEXTEXTENTS
#ವ್ಯಾಖ್ಯಾನಿಸು Xwc_ಪಠ್ಯ_List_ಗಮ್ಯ_ಪಠ್ಯ_Property XWCTEXTLISTTOTEXTPROPERTY
#ವ್ಯಾಖ್ಯಾನಿಸು Xwc_ಪಠ್ಯ_Property_ಗಮ್ಯ_ಪಠ್ಯ_List XWCTEXTPROPERTYTOTEXTLIST
#ವ್ಯಾಖ್ಯಾನಿಸು _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#ವ್ಯಾಖ್ಯಾನಿಸು _XmClearBorder _XMCLEARBORDER
#ವ್ಯಾಖ್ಯಾನಿಸು _XmConfigureObject _XMCONFIGUREOBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Destroy_ಪೋಷಕ_Callback _XMDESTROYPARENTCALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು _XmDrawArrow _XMDRAWARROW
#ವ್ಯಾಖ್ಯಾನಿಸು _XmDrawShadows _XMDRAWSHADOWS
#ವ್ಯಾಖ್ಯಾನಿಸು _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#ವ್ಯಾಖ್ಯಾನಿಸು _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#ವ್ಯಾಖ್ಯಾನಿಸು _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#ವ್ಯಾಖ್ಯಾನಿಸು _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#ವ್ಯಾಖ್ಯಾನಿಸು _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#ವ್ಯಾಖ್ಯಾನಿಸು _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Get_Widget_Ext_ದತ್ತಾಂಶ _XMGETWIDGETEXTDATA
#ವ್ಯಾಖ್ಯಾನಿಸು _XmGrabKeyboard _XMGRABKEYBOARD
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Grab_ಸೂಚಕ _XMGRABPOINTER
#ವ್ಯಾಖ್ಯಾನಿಸು _XmInheritClass _XMINHERITCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಒಳಹರಿವು_ಒಳ_Gadget _XMINPUTINGADGET
#ವ್ಯಾಖ್ಯಾನಿಸು _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_Menu_ತೆಗೆ_ಕೆಳಕ್ಕೆ _XMMENUPOPDOWN
#ವ್ಯಾಖ್ಯಾನಿಸು _XmMoveObject _XMMOVEOBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#ವ್ಯಾಖ್ಯಾನಿಸು _XmOSBuildFileList _XMOSBUILDFILELIST
#ವ್ಯಾಖ್ಯಾನಿಸು _XmOSFileCompare _XMOSFILECOMPARE
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_OSFind_ನಮೂನೆ_Part _XMOSFINDPATTERNPART
#ವ್ಯಾಖ್ಯಾನಿಸು _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#ವ್ಯಾಖ್ಯಾನಿಸು _XmPostPopupMenu _XMPOSTPOPUPMENU
#ವ್ಯಾಖ್ಯಾನಿಸು _XmPrimitiveEnter _XMPRIMITIVEENTER
#ವ್ಯಾಖ್ಯಾನಿಸು _XmPrimitiveLeave _XMPRIMITIVELEAVE
#ವ್ಯಾಖ್ಯಾನಿಸು _XmRedisplayGadgets _XMREDISPLAYGADGETS
#ವ್ಯಾಖ್ಯಾನಿಸು _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#ವ್ಯಾಖ್ಯಾನಿಸು _XmStringDraw _XMSTRINGDRAW
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_String_Get_ಪಠ್ಯ_Concat _XMSTRINGGETTEXTCONCAT
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಅಕ್ಷರಸರಣಿಗಳು _XMSTRINGS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಗಮ್ಯ_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ಗಮ್ಯ_Vertical_Pixels _XMTOVERTICALPIXELS
#ವ್ಯಾಖ್ಯಾನಿಸು Xm_ತುದಿ_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#ವ್ಯಾಖ್ಯಾನಿಸು _Xm_fastPtr _XM_FASTPTR
#ವ್ಯಾಖ್ಯಾನಿಸು _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#ವ್ಯಾಖ್ಯಾನಿಸು _XtInherit _XTINHERIT
#ವ್ಯಾಖ್ಯಾನಿಸು _XtInheritTranslations _XTINHERITTRANSLATIONS
#ವ್ಯಾಖ್ಯಾನಿಸು applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಯುಕ್ತ_Widget_Class COMPOSITEWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು shellWidgetClass SHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು ತುದಿ_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು ತುದಿ_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು vendorShellClassRec VENDORSHELLCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು wmShellWidgetClass WMSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmFormWidgetClass XMFORMWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ಚೌಕಟ್ಟು_Widget_Class XMFRAMEWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmGadgetClass XMGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmLabelGadgetClass XMLABELGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmLabelWidgetClass XMLABELWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmListWidgetClass XMLISTWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_Main_ಕಿಟಕಿ_Widget_Class XMMAINWINDOWWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmManagerClassRec XMMANAGERCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು xmManagerWidgetClass XMMANAGERWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ಸಂದೇಶ_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmPrimitiveClassRec XMPRIMITIVECLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ತಳ್ಳು_Button_Class_Rec XMPUSHBUTTONCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ತಳ್ಳು_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ತಳ್ಳು_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ_Widget_Class XMROWCOLUMNWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmSashWidgetClass XMSASHWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_Scrolled_ಕಿಟಕಿ_Class_Rec XMSCROLLEDWINDOWCLASSREC
#ವ್ಯಾಖ್ಯಾನಿಸು xm_Scrolled_ಕಿಟಕಿ_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Widget_Class XMTEXTFIELDWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xm_ಪಠ್ಯ_Widget_Class XMTEXTWIDGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#ವ್ಯಾಖ್ಯಾನಿಸು xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS_X11R6
// R5 or earlier
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ರಿಜಿಸ್ಟರ್_ಕಿಟಕಿ _XTREGISTERWINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Unregister_ಕಿಟಕಿ _XTUNREGISTERWINDOW
#ಇಲ್ಲದಿದ್ದರೆ
// R6 or later
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_ರಿಜಿಸ್ಟರ್_Drawable XTREGISTERDRAWABLE
#ವ್ಯಾಖ್ಯಾನಿಸು XtUnregisterDrawable XTUNREGISTERDRAWABLE
#ಆದರೆ_ಅಂತ್ಯ
