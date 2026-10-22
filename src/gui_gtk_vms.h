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
ਬਾਹਰੀ ਖਾਲੀ XtFree(ਅੱਖਰ*);
ਬਾਹਰੀ ਖਾਲੀ XTFREE(ਅੱਖਰ*);
// VMS_END_C_PLUS_PLUS

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAllocClassHint XALLOCCLASSHINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAllocColor XALLOCCOLOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAllocColorCells XALLOCCOLORCELLS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAlloc_ਮਾਪ_Hints XALLOCSIZEHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAllocWMHints XALLOCWMHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAutoRepeatOff XAUTOREPEATOFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAutoRepeatOn XAUTOREPEATON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XBase_Font_Name_List_ਦਾ_Font_ਸੈੱਟ XBASEFONTNAMELISTOFFONTSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XBell XBELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XBitmapPad XBITMAPPAD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XChange_ਸਰਗਰਮ_ਸੂਚਕ_Grab XCHANGEACTIVEPOINTERGRAB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XChangeGC XCHANGEGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XChangeProperty XCHANGEPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XChange_ਖਿੜਕੀ_ਗੁਣ XCHANGEWINDOWATTRIBUTES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCheckIfEvent XCHECKIFEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCheck_ਨਕਾਬ_Event XCHECKMASKEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCheckTypedEvent XCHECKTYPEDEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCheck_Typed_ਖਿੜਕੀ_Event XCHECKTYPEDWINDOWEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCheck_ਖਿੜਕੀ_Event XCHECKWINDOWEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XClearArea XCLEARAREA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XClear_ਖਿੜਕੀ XCLEARWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XClipBox XCLIPBOX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCloseDisplay XCLOSEDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCloseIM XCLOSEIM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XConfigure_ਖਿੜਕੀ XCONFIGUREWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XConvertSelection XCONVERTSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCopyArea XCOPYAREA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCopyGC XCOPYGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCopyPlane XCOPYPLANE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_Bitmap_From_ਅੰਕੜੇ XCREATEBITMAPFROMDATA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateColormap XCREATECOLORMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateFontCursor XCREATEFONTCURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_Font_ਸੈੱਟ XCREATEFONTSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateGC XCREATEGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateIC XCREATEIC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateImage XCREATEIMAGE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreatePixmap XCREATEPIXMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreatePixmapCursor XCREATEPIXMAPCURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_Pixmap_From_Bitmap_ਅੰਕੜੇ XCREATEPIXMAPFROMBITMAPDATA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreateRegion XCREATEREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_Simple_ਖਿੜਕੀ XCREATESIMPLEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_ਖਿੜਕੀ XCREATEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDefault_ਜੜ੍ਹ_ਖਿੜਕੀ XDEFAULTROOTWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDefault_Screen_ਦਾ_Display XDEFAULTSCREENOFDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDefineCursor XDEFINECURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDeleteProperty XDELETEPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDestroyIC XDESTROYIC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDestroyRegion XDESTROYREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDestroy_ਖਿੜਕੀ XDESTROYWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDisplayName XDISPLAYNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDisplay_ਦਾ_Screen XDISPLAYOFSCREEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDisplayString XDISPLAYSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawArc XDRAWARC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawImageString XDRAWIMAGESTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawImageString16 XDRAWIMAGESTRING16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDraw_ਸਤਰ XDRAWLINE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDraw_ਸਤਰਾਂ XDRAWLINES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDraw_ਬਿੰਦੂ XDRAWPOINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawPoints XDRAWPOINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawRectangle XDRAWRECTANGLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawSegments XDRAWSEGMENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawString XDRAWSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDrawString16 XDRAWSTRING16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XEmptyRegion XEMPTYREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XEqualRegion XEQUALREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XEventsQueued XEVENTSQUEUED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XExtents_ਦਾ_Font_ਸੈੱਟ XEXTENTSOFFONTSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFetch_ਬਫਰ XFETCHBUFFER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFillArc XFILLARC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFillPolygon XFILLPOLYGON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFillRectangle XFILLRECTANGLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFillRectangles XFILLRECTANGLES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFilterEvent XFILTEREVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFlush XFLUSH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFonts_ਦਾ_Font_ਸੈੱਟ XFONTSOFFONTSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFree XFREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeColormap XFREECOLORMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeColors XFREECOLORS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeCursor XFREECURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeFont XFREEFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeFontInfo XFREEFONTINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFree_Font_ਨਾਂ XFREEFONTNAMES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFree_Font_ਸੈੱਟ XFREEFONTSET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeGC XFREEGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeModifiermap XFREEMODIFIERMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreePixmap XFREEPIXMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XFreeStringList XFREESTRINGLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetAtomName XGETATOMNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetDefault XGETDEFAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGet_Error_Database_ਪਾਠ XGETERRORDATABASETEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGet_Error_ਪਾਠ XGETERRORTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetFontProperty XGETFONTPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetGCValues XGETGCVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetGeometry XGETGEOMETRY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetIconSizes XGETICONSIZES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetICValues XGETICVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetIMValues XGETIMVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetImage XGETIMAGE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetKeyboardControl XGETKEYBOARDCONTROL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetModifierMapping XGETMODIFIERMAPPING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetMotionEvents XGETMOTIONEVENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGet_ਸਾਧਾਰਨ_Hints XGETNORMALHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetSelectionOwner XGETSELECTIONOWNER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetSubImage XGETSUBIMAGE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetVisualInfo XGETVISUALINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMIconName XGETWMICONNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMProtocols XGETWMPROTOCOLS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMHints XGETWMHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMName XGETWMNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGetWMNormalHints XGETWMNORMALHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGet_ਖਿੜਕੀ_ਗੁਣ XGETWINDOWATTRIBUTES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGet_ਖਿੜਕੀ_Property XGETWINDOWPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGrabKeyboard XGRABKEYBOARD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGrab_ਸੂਚਕ XGRABPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XGrabServer XGRABSERVER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XHeight_ਦਾ_Screen XHEIGHTOFSCREEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XIconify_ਖਿੜਕੀ XICONIFYWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XIfEvent XIFEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XInternAtom XINTERNATOM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XIntersectRegion XINTERSECTREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XKeycode_ਮੰਜ਼ਿਲ_Keysym XKEYCODETOKEYSYM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XKeysym_ਮੰਜ਼ਿਲ_Keycode XKEYSYMTOKEYCODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XKeysym_ਮੰਜ਼ਿਲ_String XKEYSYMTOSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XListFonts XLISTFONTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XListFontsWithInfo XLISTFONTSWITHINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XListPixmapFormats XLISTPIXMAPFORMATS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XListProperties XLISTPROPERTIES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XLoadQueryFont XLOADQUERYFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XLookupString XLOOKUPSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XLower_ਖਿੜਕੀ XLOWERWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMapRaised XMAPRAISED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMap_ਖਿੜਕੀ XMAPWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMatchVisualInfo XMATCHVISUALINFO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMove_Resize_ਖਿੜਕੀ XMOVERESIZEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XMove_ਖਿੜਕੀ XMOVEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XNextEvent XNEXTEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XOffsetRegion XOFFSETREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XOpenDisplay XOPENDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XOpenIM XOPENIM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XParseColor XPARSECOLOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XParseGeometry XPARSEGEOMETRY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPeekEvent XPEEKEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPending XPENDING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPoint_ਵਿੱਚ_Region XPOINTINREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPolygonRegion XPOLYGONREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPut_ਪਿੱਠ_Event XPUTBACKEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPutImage XPUTIMAGE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRoot_ਖਿੜਕੀ XROOTWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XQueryColor XQUERYCOLOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XQueryColors XQUERYCOLORS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XQueryExtension XQUERYEXTENSION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XQuery_ਸੂਚਕ XQUERYPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XQuery_ਰੁੱਖ XQUERYTREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRaise_ਖਿੜਕੀ XRAISEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XReconfigureWMWindow XRECONFIGUREWMWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRect_ਵਿੱਚ_Region XRECTINREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XReparent_ਖਿੜਕੀ XREPARENTWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XResize_ਖਿੜਕੀ XRESIZEWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRestackWindows XRESTACKWINDOWS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XRoot_ਖਿੜਕੀ_ਦਾ_Screen XROOTWINDOWOFSCREEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XScreen_ਸੰਖਿਆ_ਦਾ_Screen XSCREENNUMBEROFSCREEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSelectAsyncEvent XSELECTASYNCEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSelect_Async_ਨਿਵੇਸ਼ XSELECTASYNCINPUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSelect_ਨਿਵੇਸ਼ XSELECTINPUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSendEvent XSENDEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XServerVendor XSERVERVENDOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetBackground XSETBACKGROUND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetClassHint XSETCLASSHINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_Clip_ਨਕਾਬ XSETCLIPMASK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetClipOrigin XSETCLIPORIGIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetClipRectangles XSETCLIPRECTANGLES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_Close_ਥੱਲੇ_ਬਹੁਲਕ XSETCLOSEDOWNMODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetCommand XSETCOMMAND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetDashes XSETDASHES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetErrorHandler XSETERRORHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetFillStyle XSETFILLSTYLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetFont XSETFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetForeground XSETFOREGROUND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetFunction XSETFUNCTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetICFocus XSETICFOCUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetICValues XSETICVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetIOErrorHandler XSETIOERRORHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਨਿਵੇਸ਼_Focus XSETINPUTFOCUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਸਤਰ_ਗੁਣ XSETLINEATTRIBUTES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetLocaleModifiers XSETLOCALEMODIFIERS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਸਾਧਾਰਨ_Hints XSETNORMALHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetRegion XSETREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetSelectionOwner XSETSELECTIONOWNER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetStipple XSETSTIPPLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_Subwindow_ਬਹੁਲਕ XSETSUBWINDOWMODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetTSOrigin XSETTSORIGIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetTile XSETTILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetTransientForHint XSETTRANSIENTFORHINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMHints XSETWMHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMIconName XSETWMICONNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMName XSETWMNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMNormalHints XSETWMNORMALHINTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMProperties XSETWMPROPERTIES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSetWMProtocols XSETWMPROTOCOLS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਖਿੜਕੀ_Background XSETWINDOWBACKGROUND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਖਿੜਕੀ_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSet_ਖਿੜਕੀ_Colormap XSETWINDOWCOLORMAP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XShape_Combine_ਨਕਾਬ XSHAPECOMBINEMASK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XShapeGetRectangles XSHAPEGETRECTANGLES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XShrinkRegion XSHRINKREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XStore_ਬਫਰ XSTOREBUFFER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XStoreColor XSTORECOLOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XStoreColors XSTORECOLORS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XStoreName XSTORENAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XString_ਮੰਜ਼ਿਲ_Keysym XSTRINGTOKEYSYM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSubtractRegion XSUBTRACTREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSupportsLocale XSUPPORTSLOCALE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSync XSYNC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XSynchronize XSYNCHRONIZE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XTextExtents XTEXTEXTENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XTextExtents16 XTEXTEXTENTS16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XText_ਚੌੜਾਈ XTEXTWIDTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XTextWidth16 XTEXTWIDTH16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XTranslateCoordinates XTRANSLATECOORDINATES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUndefineCursor XUNDEFINECURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUngrabKeyboard XUNGRABKEYBOARD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUngrab_ਸੂਚਕ XUNGRABPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUngrabServer XUNGRABSERVER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUnionRectWithRegion XUNIONRECTWITHREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUnionRegion XUNIONREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUnmap_ਖਿੜਕੀ XUNMAPWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XUnregisterFilter _XUNREGISTERFILTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XUnsetICFocus XUNSETICFOCUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XVaCreateNestedList XVACREATENESTEDLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XVisualIDFromVisual XVISUALIDFROMVISUAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XWarp_ਸੂਚਕ XWARPPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XWidth_ਦਾ_Screen XWIDTHOFSCREEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XWindowEvent XWINDOWEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XWithdraw_ਖਿੜਕੀ XWITHDRAWWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XXorRegion XXORREGION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਜੋੜੋ_Protocol_Callback XMADDPROTOCOLCALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਜੋੜੋ_Protocols XMADDPROTOCOLS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmChangeColor XMCHANGECOLOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmClipboardCopy XMCLIPBOARDCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Clipboard_ਸਿਰਾ_Copy XMCLIPBOARDENDCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Clipboard_Inquire_ਲੰਮਾਈ XMCLIPBOARDINQUIRELENGTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Clipboard_ਤਾਲਾ XMCLIPBOARDLOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Clipboard_ਸ਼ੁਰੂ_Copy XMCLIPBOARDSTARTCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmClipboardUnlock XMCLIPBOARDUNLOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateArrowButton XMCREATEARROWBUTTON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateCascadeButton XMCREATECASCADEBUTTON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateDialogShell XMCREATEDIALOGSHELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateDragIcon XMCREATEDRAGICON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateDrawingArea XMCREATEDRAWINGAREA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateDrawnButton XMCREATEDRAWNBUTTON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateForm XMCREATEFORM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateFormDialog XMCREATEFORMDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਚੌਖਟਾ XMCREATEFRAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateLabelGadget XMCREATELABELGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_Main_ਖਿੜਕੀ XMCREATEMAINWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateMenuBar XMCREATEMENUBAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਸੁਨੇਹਾ_Box XMCREATEMESSAGEBOX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਸੁਨੇਹਾ_Dialog XMCREATEMESSAGEDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateOptionMenu XMCREATEOPTIONMENU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_Paned_ਖਿੜਕੀ XMCREATEPANEDWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreatePopupMenu XMCREATEPOPUPMENU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreatePromptDialog XMCREATEPROMPTDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਧੱਕੋ_Button XMCREATEPUSHBUTTON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਧੱਕੋ_Button_Gadget XMCREATEPUSHBUTTONGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateRadioBox XMCREATERADIOBOX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਪੰਕਤੀ_ਕਾਲਮ XMCREATEROWCOLUMN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateScale XMCREATESCALE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateScrollBar XMCREATESCROLLBAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateScrolledList XMCREATESCROLLEDLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_Scrolled_ਪਾਠ XMCREATESCROLLEDTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_Scrolled_ਖਿੜਕੀ XMCREATESCROLLEDWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateSeparator XMCREATESEPARATOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਪਾਠ XMCREATETEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Create_ਪਾਠ_ਖੇਤਰ XMCREATETEXTFIELD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateToggleButton XMCREATETOGGLEBUTTON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Drag_ਸ਼ੁਰੂ XMDRAGSTART
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Drop_Site_ਰਜਿਸਟਰ XMDROPSITEREGISTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmDropSiteUnregister XMDROPSITEUNREGISTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmDropSiteUpdate XMDROPSITEUPDATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Drop_Transfer_ਸ਼ੁਰੂ XMDROPTRANSFERSTART
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_File_Selection_Box_Get_ਬੱਚਾ XMFILESELECTIONBOXGETCHILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_File_Selection_Do_ਖੋਜ XMFILESELECTIONDOSEARCH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_Append_ਇੰਦਰਾਜ XMFONTLISTAPPENDENTRY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmFontListCopy XMFONTLISTCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmFontListCreate XMFONTLISTCREATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਇੰਦਰਾਜ_Create XMFONTLISTENTRYCREATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਇੰਦਰਾਜ_Free XMFONTLISTENTRYFREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਇੰਦਰਾਜ_Get_Font XMFONTLISTENTRYGETFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਇੰਦਰਾਜ_Get_ਟੈਗ XMFONTLISTENTRYGETTAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਇੰਦਰਾਜ_ਲੱਦੋ XMFONTLISTENTRYLOAD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmFontListFree XMFONTLISTFREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_Get_ਅਗਲਾ_Font XMFONTLISTGETNEXTFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Font_List_ਅਗਲਾ_ਇੰਦਰਾਜ XMFONTLISTNEXTENTRY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmGetColors XMGETCOLORS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmGetFocusWidget XMGETFOCUSWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmGetMenuCursor XMGETMENUCURSOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Get_Pixmap_By_ਡੂੰਘਾਈ XMGETPIXMAPBYDEPTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmGetTearOffControl XMGETTEAROFFCONTROL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmGetXmDisplay XMGETXMDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmImMbLookupString XMIMMBLOOKUPSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Im_ਰਜਿਸਟਰ XMIMREGISTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Im_ਸੈੱਟ_Focus_ਮੁੱਲਾਂ XMIMSETFOCUSVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Im_ਸੈੱਟ_ਮੁੱਲਾਂ XMIMSETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmImUnregister XMIMUNREGISTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmImUnsetFocus XMIMUNSETFOCUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmInternAtom XMINTERNATOM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmIsMotifWMRunning XMISMOTIFWMRUNNING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਜੋੜੋ_ਵਸਤੂ XMLISTADDITEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਜੋੜੋ_ਵਸਤੂ_Unselected XMLISTADDITEMUNSELECTED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਜੋੜੋ_ਚੀਜ਼ਾਂ_Unselected XMLISTADDITEMSUNSELECTED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Delete_All_ਚੀਜ਼ਾਂ XMLISTDELETEALLITEMS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Delete_ਚੀਜ਼ਾਂ_Pos XMLISTDELETEITEMSPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmListDeletePos XMLISTDELETEPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Deselect_All_ਚੀਜ਼ਾਂ XMLISTDESELECTALLITEMS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmListDeselectPos XMLISTDESELECTPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Get_Kbd_ਵਸਤੂ_Pos XMLISTGETKBDITEMPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Get_ਮੇਲ_Pos XMLISTGETMATCHPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmListGetSelectedPos XMLISTGETSELECTEDPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmListPosSelected XMLISTPOSSELECTED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_Select_ਵਸਤੂ XMLISTSELECTITEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmListSelectPos XMLISTSELECTPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਸੈੱਟ_Bottom_Pos XMLISTSETBOTTOMPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਸੈੱਟ_ਵਸਤੂ XMLISTSETITEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਸੈੱਟ_Kbd_ਵਸਤੂ_Pos XMLISTSETKBDITEMPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_List_ਸੈੱਟ_Pos XMLISTSETPOS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Main_ਖਿੜਕੀ_ਸੈੱਟ_Areas XMMAINWINDOWSETAREAS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmMenuPosition XMMENUPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਸੁਨੇਹਾ_Box_Get_ਬੱਚਾ XMMESSAGEBOXGETCHILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmOptionButtonGadget XMOPTIONBUTTONGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmOptionLabelGadget XMOPTIONLABELGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmProcessTraversal XMPROCESSTRAVERSAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmQmotif XMQMOTIF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmRepTypeGetId XMREPTYPEGETID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Rep_Type_Get_ਰਿਕਾਰਡ XMREPTYPEGETRECORD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Rep_Type_ਰਜਿਸਟਰ XMREPTYPEREGISTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Rep_Type_Valid_ਮੁੱਲ XMREPTYPEVALIDVALUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Scroll_Bar_ਸੈੱਟ_ਮੁੱਲਾਂ XMSCROLLBARSETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Scrolled_ਖਿੜਕੀ_ਸੈੱਟ_Areas XMSCROLLEDWINDOWSETAREAS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Selection_Box_Get_ਬੱਚਾ XMSELECTIONBOXGETCHILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringByteCompare XMSTRINGBYTECOMPARE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCompare XMSTRINGCOMPARE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringConcat XMSTRINGCONCAT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCopy XMSTRINGCOPY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCreate XMSTRINGCREATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCreateLtoR XMSTRINGCREATELTOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringCreateSimple XMSTRINGCREATESIMPLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringDraw XMSTRINGDRAW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringExtent XMSTRINGEXTENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringFree XMSTRINGFREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringFreeContext XMSTRINGFREECONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringGetLtoR XMSTRINGGETLTOR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_String_Get_ਅਗਲਾ_Component XMSTRINGGETNEXTCOMPONENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_String_Get_ਅਗਲਾ_Segment XMSTRINGGETNEXTSEGMENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringInitContext XMSTRINGINITCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_String_ਲੰਮਾਈ XMSTRINGLENGTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringLtoRCreate XMSTRINGLTORCREATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringNConcat XMSTRINGNCONCAT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_String_ਚੌੜਾਈ XMSTRINGWIDTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Clear_Selection XMTEXTCLEARSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Get_Editable XMTEXTFIELDGETEDITABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Get_ਆਖਰੀ_Position XMTEXTFIELDGETLASTPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Get_Selection XMTEXTFIELDGETSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Get_String XMTEXTFIELDGETSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_ਪਾਓ XMTEXTFIELDINSERT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_Remove XMTEXTFIELDREMOVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_Selection XMTEXTFIELDSETSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_String XMTEXTFIELDSETSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_ਆਖਰੀ_Position XMTEXTGETLASTPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_Max_ਲੰਮਾਈ XMTEXTGETMAXLENGTH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_Selection XMTEXTGETSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Get_String XMTEXTGETSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਪਾਓ XMTEXTINSERT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Remove XMTEXTREMOVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_Replace XMTEXTREPLACE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਸੈੱਟ_Cursor_Position XMTEXTSETCURSORPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਸੈੱਟ_Highlight XMTEXTSETHIGHLIGHT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਸੈੱਟ_Insertion_Position XMTEXTSETINSERTIONPOSITION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਸੈੱਟ_Selection XMTEXTSETSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਪਾਠ_ਸੈੱਟ_String XMTEXTSETSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Toggle_Button_Gadget_ਸੈੱਟ_State XMTOGGLEBUTTONGADGETSETSTATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Toggle_Button_ਸੈੱਟ_State XMTOGGLEBUTTONSETSTATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmUpdateDisplay XMUPDATEDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmbDrawString XMBDRAWSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmbLookupString XMBLOOKUPSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XmbResetIC XMBRESETIC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmb_ਸੈੱਟ_WMProperties XMBSETWMPROPERTIES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmb_ਪਾਠ_Escapement XMBTEXTESCAPEMENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmb_ਪਾਠ_Extents XMBTEXTEXTENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmb_ਪਾਠ_List_ਮੰਜ਼ਿਲ_ਪਾਠ_Property XMBTEXTLISTTOTEXTPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmb_ਪਾਠ_Property_ਮੰਜ਼ਿਲ_ਪਾਠ_List XMBTEXTPROPERTYTOTEXTLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmu_Client_ਖਿੜਕੀ XMUCLIENTWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xmu_Print_Default_Error_ਸੁਨੇਹਾ XMUPRINTDEFAULTERRORMESSAGE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XrmGetDatabase XRMGETDATABASE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XrmGetResource XRMGETRESOURCE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XrmPutStringResource XRMPUTSTRINGRESOURCE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xrm_Quark_ਮੰਜ਼ਿਲ_String XRMQUARKTOSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xrm_String_ਮੰਜ਼ਿਲ_Quark XRMSTRINGTOQUARK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਜੋੜੋ_Callback XTADDCALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਜੋੜੋ_Callbacks XTADDCALLBACKS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਜੋੜੋ_Event_Handler XTADDEVENTHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਜੋੜੋ_Grab XTADDGRAB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAllocateGC XTALLOCATEGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਜੋੜੋ_Actions XTAPPADDACTIONS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਜੋੜੋ_ਨਿਵੇਸ਼ XTAPPADDINPUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ XTAPPADDTIMEOUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppCreateShell XTAPPCREATESHELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppInitialize XTAPPINITIALIZE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਅਗਲਾ_Event XTAPPNEXTEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppPeekEvent XTAPPPEEKEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppPending XTAPPPENDING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppProcessEvent XTAPPPROCESSEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਸੈੱਟ_Error_Handler XTAPPSETERRORHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਸੈੱਟ_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਸੈੱਟ_Warning_Handler XTAPPSETWARNINGHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_App_ਸੈੱਟ_Warning_ਸੁਨੇਹਾ_Handler XTAPPSETWARNINGMSGHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtAppWarning XTAPPWARNING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCallActionProc XTCALLACTIONPROC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCallCallbackList XTCALLCALLBACKLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCallCallbacks XTCALLCALLBACKS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtConfigureWidget XTCONFIGUREWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtConvertAndStore XTCONVERTANDSTORE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCreatePopupShell XTCREATEPOPUPSHELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCreateWidget XTCREATEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtDatabase XTDATABASE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtDestroyWidget XTDESTROYWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtDisownSelection XTDISOWNSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtDispatchEvent XTDISPATCHEVENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Display_ਦਾ_Object XTDISPLAYOFOBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Display_ਮੰਜ਼ਿਲ_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtFree XTFREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetActionKeysym XTGETACTIONKEYSYM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetActionList XTGETACTIONLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetGC XTGETGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Get_Multi_Click_ਸਮਾਂ XTGETMULTICLICKTIME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Get_Selection_ਮੁੱਲ XTGETSELECTIONVALUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Get_Selection_ਮੁੱਲਾਂ XTGETSELECTIONVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGetSubresources XTGETSUBRESOURCES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Get_ਮੁੱਲਾਂ XTGETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtGrabKeyboard XTGRABKEYBOARD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Grab_ਸੂਚਕ XTGRABPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtHasCallbacks XTHASCALLBACKS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਪਾਓ_Event_Handler XTINSERTEVENTHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtIsManaged XTISMANAGED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtIsObject XTISOBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtIsSensitive XTISSENSITIVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtIsSubclass XTISSUBCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਆਖਰੀ_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtMakeResizeRequest XTMAKERESIZEREQUEST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtMalloc XTMALLOC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Manage_ਬੱਚਾ XTMANAGECHILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtManageChildren XTMANAGECHILDREN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਮਿਲਾਓ_Arg_Lists XTMERGEARGLISTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtMoveWidget XTMOVEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtName XTNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Name_ਮੰਜ਼ਿਲ_Widget XTNAMETOWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtOpenDisplay XTOPENDISPLAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtOverrideTranslations XTOVERRIDETRANSLATIONS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtOwnSelection XTOWNSELECTION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਮਾਪਾ XTPARENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Parse_Translation_ਸਾਰਣੀ XTPARSETRANSLATIONTABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtPopdown XTPOPDOWN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtPopup XTPOPUP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtQueryGeometry XTQUERYGEOMETRY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRealizeWidget XTREALIZEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRealloc XTREALLOC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtReleaseGC XTRELEASEGC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRemoveCallback XTREMOVECALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRemoveEventHandler XTREMOVEEVENTHANDLER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtRemoveGrab XTREMOVEGRAB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Remove_ਨਿਵੇਸ਼ XTREMOVEINPUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ XTREMOVETIMEOUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtResizeWidget XTRESIZEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtResolvePathname XTRESOLVEPATHNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਸੈੱਟ_Keyboard_Focus XTSETKEYBOARDFOCUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਸੈੱਟ_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਸੈੱਟ_Sensitive XTSETSENSITIVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਸੈੱਟ_Type_Converter XTSETTYPECONVERTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਸੈੱਟ_ਮੁੱਲਾਂ XTSETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Shell_ਲੜੀਆਂ XTSHELLSTRINGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਲੜੀਆਂ XTSTRINGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtToolkitInitialize XTTOOLKITINITIALIZE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtTranslateCoords XTTRANSLATECOORDS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtTranslateKeycode XTTRANSLATEKEYCODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtUngrabKeyboard XTUNGRABKEYBOARD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Ungrab_ਸੂਚਕ XTUNGRABPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Unmanage_ਬੱਚਾ XTUNMANAGECHILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtUnmanageChildren XTUNMANAGECHILDREN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtUnrealizeWidget XTUNREALIZEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtVaAppCreateShell XTVAAPPCREATESHELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtVaCreateWidget XTVACREATEWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Va_Get_ਮੁੱਲਾਂ XTVAGETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ XTVASETVALUES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtWarning XTWARNING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Widget_ਮੰਜ਼ਿਲ_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਖਿੜਕੀ XTWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਖਿੜਕੀ_ਦਾ_Object XTWINDOWOFOBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਖਿੜਕੀ_ਮੰਜ਼ਿਲ_Widget XTWINDOWTOWIDGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XwcDrawString XWCDRAWSTRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XwcFreeStringList XWCFREESTRINGLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xwc_ਪਾਠ_Escapement XWCTEXTESCAPEMENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xwc_ਪਾਠ_Extents XWCTEXTEXTENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xwc_ਪਾਠ_List_ਮੰਜ਼ਿਲ_ਪਾਠ_Property XWCTEXTLISTTOTEXTPROPERTY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xwc_ਪਾਠ_Property_ਮੰਜ਼ਿਲ_ਪਾਠ_List XWCTEXTPROPERTYTOTEXTLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmClearBorder _XMCLEARBORDER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmConfigureObject _XMCONFIGUREOBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Destroy_ਮਾਪਾ_Callback _XMDESTROYPARENTCALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmDrawArrow _XMDRAWARROW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmDrawShadows _XMDRAWSHADOWS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Get_Widget_Ext_ਅੰਕੜੇ _XMGETWIDGETEXTDATA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmGrabKeyboard _XMGRABKEYBOARD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Grab_ਸੂਚਕ _XMGRABPOINTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmInheritClass _XMINHERITCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਨਿਵੇਸ਼_ਵਿੱਚ_Gadget _XMINPUTINGADGET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_Menu_ਕੱਢੋ_ਥੱਲੇ _XMMENUPOPDOWN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmMoveObject _XMMOVEOBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmOSBuildFileList _XMOSBUILDFILELIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmOSFileCompare _XMOSFILECOMPARE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_OSFind_ਨਮੂਨਾ_Part _XMOSFINDPATTERNPART
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmPostPopupMenu _XMPOSTPOPUPMENU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmPrimitiveEnter _XMPRIMITIVEENTER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmPrimitiveLeave _XMPRIMITIVELEAVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmRedisplayGadgets _XMREDISPLAYGADGETS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XmStringDraw _XMSTRINGDRAW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_String_Get_ਪਾਠ_Concat _XMSTRINGGETTEXTCONCAT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਲੜੀਆਂ _XMSTRINGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਮੰਜ਼ਿਲ_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਮੰਜ਼ਿਲ_Vertical_Pixels _XMTOVERTICALPIXELS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xm_ਸਿਖਰ_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _Xm_fastPtr _XM_FASTPTR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XtInherit _XTINHERIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _XtInheritTranslations _XTINHERITTRANSLATIONS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਯੁਕਤ_Widget_Class COMPOSITEWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ shellWidgetClass SHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਖਰ_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਖਰ_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vendorShellClassRec VENDORSHELLCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ wmShellWidgetClass WMSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmFormWidgetClass XMFORMWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਚੌਖਟਾ_Widget_Class XMFRAMEWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmGadgetClass XMGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmLabelGadgetClass XMLABELGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmLabelWidgetClass XMLABELWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmListWidgetClass XMLISTWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_Main_ਖਿੜਕੀ_Widget_Class XMMAINWINDOWWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmManagerClassRec XMMANAGERCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmManagerWidgetClass XMMANAGERWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਸੁਨੇਹਾ_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmPrimitiveClassRec XMPRIMITIVECLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਧੱਕੋ_Button_Class_Rec XMPUSHBUTTONCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਧੱਕੋ_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਧੱਕੋ_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਪੰਕਤੀ_ਕਾਲਮ_Widget_Class XMROWCOLUMNWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmSashWidgetClass XMSASHWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_Scrolled_ਖਿੜਕੀ_Class_Rec XMSCROLLEDWINDOWCLASSREC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_Scrolled_ਖਿੜਕੀ_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਪਾਠ_ਖੇਤਰ_Widget_Class XMTEXTFIELDWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xm_ਪਾਠ_Widget_Class XMTEXTWIDGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS_X11R6
// R5 or earlier
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਰਜਿਸਟਰ_ਖਿੜਕੀ _XTREGISTERWINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Unregister_ਖਿੜਕੀ _XTUNREGISTERWINDOW
#ਨਹੀਂਤਾਂ
// R6 or later
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_ਰਜਿਸਟਰ_Drawable XTREGISTERDRAWABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtUnregisterDrawable XTUNREGISTERDRAWABLE
#ਜੇ_ਅੰਤ
