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
خارجي فراغ XtFree(حرف*);
خارجي فراغ XTFREE(حرف*);
// VMS_END_C_PLUS_PLUS

#وصف_ڪريو _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#وصف_ڪريو XAllocClassHint XALLOCCLASSHINT
#وصف_ڪريو XAllocColor XALLOCCOLOR
#وصف_ڪريو XAllocColorCells XALLOCCOLORCELLS
#وصف_ڪريو XAlloc_سائيز_Hints XALLOCSIZEHINTS
#وصف_ڪريو XAllocWMHints XALLOCWMHINTS
#وصف_ڪريو XAutoRepeatOff XAUTOREPEATOFF
#وصف_ڪريو XAutoRepeatOn XAUTOREPEATON
#وصف_ڪريو XBase_Font_Name_List_جو_Font_سيٽ XBASEFONTNAMELISTOFFONTSET
#وصف_ڪريو XBell XBELL
#وصف_ڪريو XBitmapPad XBITMAPPAD
#وصف_ڪريو XChange_سرگرم_اشارو_Grab XCHANGEACTIVEPOINTERGRAB
#وصف_ڪريو XChangeGC XCHANGEGC
#وصف_ڪريو XChangeProperty XCHANGEPROPERTY
#وصف_ڪريو XChange_دري_خاصيتون XCHANGEWINDOWATTRIBUTES
#وصف_ڪريو XCheckIfEvent XCHECKIFEVENT
#وصف_ڪريو XCheck_نقاب_Event XCHECKMASKEVENT
#وصف_ڪريو XCheckTypedEvent XCHECKTYPEDEVENT
#وصف_ڪريو XCheck_Typed_دري_Event XCHECKTYPEDWINDOWEVENT
#وصف_ڪريو XCheck_دري_Event XCHECKWINDOWEVENT
#وصف_ڪريو XClearArea XCLEARAREA
#وصف_ڪريو XClear_دري XCLEARWINDOW
#وصف_ڪريو XClipBox XCLIPBOX
#وصف_ڪريو XCloseDisplay XCLOSEDISPLAY
#وصف_ڪريو XCloseIM XCLOSEIM
#وصف_ڪريو XConfigure_دري XCONFIGUREWINDOW
#وصف_ڪريو XConvertSelection XCONVERTSELECTION
#وصف_ڪريو XCopyArea XCOPYAREA
#وصف_ڪريو XCopyGC XCOPYGC
#وصف_ڪريو XCopyPlane XCOPYPLANE
#وصف_ڪريو XCreate_Bitmap_From_معلومات XCREATEBITMAPFROMDATA
#وصف_ڪريو XCreateColormap XCREATECOLORMAP
#وصف_ڪريو XCreateFontCursor XCREATEFONTCURSOR
#وصف_ڪريو XCreate_Font_سيٽ XCREATEFONTSET
#وصف_ڪريو XCreateGC XCREATEGC
#وصف_ڪريو XCreateIC XCREATEIC
#وصف_ڪريو XCreateImage XCREATEIMAGE
#وصف_ڪريو XCreatePixmap XCREATEPIXMAP
#وصف_ڪريو XCreatePixmapCursor XCREATEPIXMAPCURSOR
#وصف_ڪريو XCreate_Pixmap_From_Bitmap_معلومات XCREATEPIXMAPFROMBITMAPDATA
#وصف_ڪريو XCreateRegion XCREATEREGION
#وصف_ڪريو XCreate_Simple_دري XCREATESIMPLEWINDOW
#وصف_ڪريو XCreate_دري XCREATEWINDOW
#وصف_ڪريو XDefault_پاڙ_دري XDEFAULTROOTWINDOW
#وصف_ڪريو XDefault_Screen_جو_Display XDEFAULTSCREENOFDISPLAY
#وصف_ڪريو XDefineCursor XDEFINECURSOR
#وصف_ڪريو XDeleteProperty XDELETEPROPERTY
#وصف_ڪريو XDestroyIC XDESTROYIC
#وصف_ڪريو XDestroyRegion XDESTROYREGION
#وصف_ڪريو XDestroy_دري XDESTROYWINDOW
#وصف_ڪريو XDisplayName XDISPLAYNAME
#وصف_ڪريو XDisplay_جو_Screen XDISPLAYOFSCREEN
#وصف_ڪريو XDisplayString XDISPLAYSTRING
#وصف_ڪريو XDrawArc XDRAWARC
#وصف_ڪريو XDrawImageString XDRAWIMAGESTRING
#وصف_ڪريو XDrawImageString16 XDRAWIMAGESTRING16
#وصف_ڪريو XDraw_سٽ XDRAWLINE
#وصف_ڪريو XDraw_سٽون XDRAWLINES
#وصف_ڪريو XDraw_نقطو XDRAWPOINT
#وصف_ڪريو XDrawPoints XDRAWPOINTS
#وصف_ڪريو XDrawRectangle XDRAWRECTANGLE
#وصف_ڪريو XDrawSegments XDRAWSEGMENTS
#وصف_ڪريو XDrawString XDRAWSTRING
#وصف_ڪريو XDrawString16 XDRAWSTRING16
#وصف_ڪريو XEmptyRegion XEMPTYREGION
#وصف_ڪريو XEqualRegion XEQUALREGION
#وصف_ڪريو XEventsQueued XEVENTSQUEUED
#وصف_ڪريو XExtents_جو_Font_سيٽ XEXTENTSOFFONTSET
#وصف_ڪريو XFetch_بفر XFETCHBUFFER
#وصف_ڪريو XFillArc XFILLARC
#وصف_ڪريو XFillPolygon XFILLPOLYGON
#وصف_ڪريو XFillRectangle XFILLRECTANGLE
#وصف_ڪريو XFillRectangles XFILLRECTANGLES
#وصف_ڪريو XFilterEvent XFILTEREVENT
#وصف_ڪريو XFlush XFLUSH
#وصف_ڪريو XFonts_جو_Font_سيٽ XFONTSOFFONTSET
#وصف_ڪريو XFree XFREE
#وصف_ڪريو XFreeColormap XFREECOLORMAP
#وصف_ڪريو XFreeColors XFREECOLORS
#وصف_ڪريو XFreeCursor XFREECURSOR
#وصف_ڪريو XFreeFont XFREEFONT
#وصف_ڪريو XFreeFontInfo XFREEFONTINFO
#وصف_ڪريو XFree_Font_نالا XFREEFONTNAMES
#وصف_ڪريو XFree_Font_سيٽ XFREEFONTSET
#وصف_ڪريو XFreeGC XFREEGC
#وصف_ڪريو XFreeModifiermap XFREEMODIFIERMAP
#وصف_ڪريو XFreePixmap XFREEPIXMAP
#وصف_ڪريو XFreeStringList XFREESTRINGLIST
#وصف_ڪريو XGetAtomName XGETATOMNAME
#وصف_ڪريو XGetDefault XGETDEFAULT
#وصف_ڪريو XGet_Error_Database_متن XGETERRORDATABASETEXT
#وصف_ڪريو XGet_Error_متن XGETERRORTEXT
#وصف_ڪريو XGetFontProperty XGETFONTPROPERTY
#وصف_ڪريو XGetGCValues XGETGCVALUES
#وصف_ڪريو XGetGeometry XGETGEOMETRY
#وصف_ڪريو XGetIconSizes XGETICONSIZES
#وصف_ڪريو XGetICValues XGETICVALUES
#وصف_ڪريو XGetIMValues XGETIMVALUES
#وصف_ڪريو XGetImage XGETIMAGE
#وصف_ڪريو XGetKeyboardControl XGETKEYBOARDCONTROL
#وصف_ڪريو XGetModifierMapping XGETMODIFIERMAPPING
#وصف_ڪريو XGetMotionEvents XGETMOTIONEVENTS
#وصف_ڪريو XGet_عام_Hints XGETNORMALHINTS
#وصف_ڪريو XGetSelectionOwner XGETSELECTIONOWNER
#وصف_ڪريو XGetSubImage XGETSUBIMAGE
#وصف_ڪريو XGetVisualInfo XGETVISUALINFO
#وصف_ڪريو XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#وصف_ڪريو XGetWMIconName XGETWMICONNAME
#وصف_ڪريو XGetWMProtocols XGETWMPROTOCOLS
#وصف_ڪريو XGetWMHints XGETWMHINTS
#وصف_ڪريو XGetWMName XGETWMNAME
#وصف_ڪريو XGetWMNormalHints XGETWMNORMALHINTS
#وصف_ڪريو XGet_دري_خاصيتون XGETWINDOWATTRIBUTES
#وصف_ڪريو XGet_دري_Property XGETWINDOWPROPERTY
#وصف_ڪريو XGrabKeyboard XGRABKEYBOARD
#وصف_ڪريو XGrab_اشارو XGRABPOINTER
#وصف_ڪريو XGrabServer XGRABSERVER
#وصف_ڪريو XHeight_جو_Screen XHEIGHTOFSCREEN
#وصف_ڪريو XIconify_دري XICONIFYWINDOW
#وصف_ڪريو XIfEvent XIFEVENT
#وصف_ڪريو XInternAtom XINTERNATOM
#وصف_ڪريو XIntersectRegion XINTERSECTREGION
#وصف_ڪريو XKeycode_منزل_Keysym XKEYCODETOKEYSYM
#وصف_ڪريو XKeysym_منزل_Keycode XKEYSYMTOKEYCODE
#وصف_ڪريو XKeysym_منزل_String XKEYSYMTOSTRING
#وصف_ڪريو XListFonts XLISTFONTS
#وصف_ڪريو XListFontsWithInfo XLISTFONTSWITHINFO
#وصف_ڪريو XListPixmapFormats XLISTPIXMAPFORMATS
#وصف_ڪريو XListProperties XLISTPROPERTIES
#وصف_ڪريو XLoadQueryFont XLOADQUERYFONT
#وصف_ڪريو XLookupString XLOOKUPSTRING
#وصف_ڪريو XLower_دري XLOWERWINDOW
#وصف_ڪريو XMapRaised XMAPRAISED
#وصف_ڪريو XMap_دري XMAPWINDOW
#وصف_ڪريو XMatchVisualInfo XMATCHVISUALINFO
#وصف_ڪريو XMove_Resize_دري XMOVERESIZEWINDOW
#وصف_ڪريو XMove_دري XMOVEWINDOW
#وصف_ڪريو XNextEvent XNEXTEVENT
#وصف_ڪريو XOffsetRegion XOFFSETREGION
#وصف_ڪريو XOpenDisplay XOPENDISPLAY
#وصف_ڪريو XOpenIM XOPENIM
#وصف_ڪريو XParseColor XPARSECOLOR
#وصف_ڪريو XParseGeometry XPARSEGEOMETRY
#وصف_ڪريو XPeekEvent XPEEKEVENT
#وصف_ڪريو XPending XPENDING
#وصف_ڪريو XPoint_اندريون_Region XPOINTINREGION
#وصف_ڪريو XPolygonRegion XPOLYGONREGION
#وصف_ڪريو XPut_پٺ_Event XPUTBACKEVENT
#وصف_ڪريو XPutImage XPUTIMAGE
#وصف_ڪريو XRoot_دري XROOTWINDOW
#وصف_ڪريو XQueryColor XQUERYCOLOR
#وصف_ڪريو XQueryColors XQUERYCOLORS
#وصف_ڪريو XQueryExtension XQUERYEXTENSION
#وصف_ڪريو XQuery_اشارو XQUERYPOINTER
#وصف_ڪريو XQuery_وڻ XQUERYTREE
#وصف_ڪريو XRaise_دري XRAISEWINDOW
#وصف_ڪريو XReconfigureWMWindow XRECONFIGUREWMWINDOW
#وصف_ڪريو XRect_اندريون_Region XRECTINREGION
#وصف_ڪريو XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#وصف_ڪريو XReparent_دري XREPARENTWINDOW
#وصف_ڪريو XResize_دري XRESIZEWINDOW
#وصف_ڪريو XRestackWindows XRESTACKWINDOWS
#وصف_ڪريو XRoot_دري_جو_Screen XROOTWINDOWOFSCREEN
#وصف_ڪريو XScreen_عدد_جو_Screen XSCREENNUMBEROFSCREEN
#وصف_ڪريو XSelectAsyncEvent XSELECTASYNCEVENT
#وصف_ڪريو XSelect_Async_داخلا XSELECTASYNCINPUT
#وصف_ڪريو XSelect_داخلا XSELECTINPUT
#وصف_ڪريو XSendEvent XSENDEVENT
#وصف_ڪريو XServerVendor XSERVERVENDOR
#وصف_ڪريو XSetBackground XSETBACKGROUND
#وصف_ڪريو XSetClassHint XSETCLASSHINT
#وصف_ڪريو XSet_Clip_نقاب XSETCLIPMASK
#وصف_ڪريو XSetClipOrigin XSETCLIPORIGIN
#وصف_ڪريو XSetClipRectangles XSETCLIPRECTANGLES
#وصف_ڪريو XSet_Close_هيٺ_ڏانهن_گهڻائي XSETCLOSEDOWNMODE
#وصف_ڪريو XSetCommand XSETCOMMAND
#وصف_ڪريو XSetDashes XSETDASHES
#وصف_ڪريو XSetErrorHandler XSETERRORHANDLER
#وصف_ڪريو XSetFillStyle XSETFILLSTYLE
#وصف_ڪريو XSetFont XSETFONT
#وصف_ڪريو XSetForeground XSETFOREGROUND
#وصف_ڪريو XSetFunction XSETFUNCTION
#وصف_ڪريو XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#وصف_ڪريو XSetICFocus XSETICFOCUS
#وصف_ڪريو XSetICValues XSETICVALUES
#وصف_ڪريو XSetIOErrorHandler XSETIOERRORHANDLER
#وصف_ڪريو XSet_داخلا_Focus XSETINPUTFOCUS
#وصف_ڪريو XSet_سٽ_خاصيتون XSETLINEATTRIBUTES
#وصف_ڪريو XSetLocaleModifiers XSETLOCALEMODIFIERS
#وصف_ڪريو XSet_عام_Hints XSETNORMALHINTS
#وصف_ڪريو XSetRegion XSETREGION
#وصف_ڪريو XSetSelectionOwner XSETSELECTIONOWNER
#وصف_ڪريو XSetStipple XSETSTIPPLE
#وصف_ڪريو XSet_Subwindow_گهڻائي XSETSUBWINDOWMODE
#وصف_ڪريو XSetTSOrigin XSETTSORIGIN
#وصف_ڪريو XSetTile XSETTILE
#وصف_ڪريو XSetTransientForHint XSETTRANSIENTFORHINT
#وصف_ڪريو XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#وصف_ڪريو XSetWMHints XSETWMHINTS
#وصف_ڪريو XSetWMIconName XSETWMICONNAME
#وصف_ڪريو XSetWMName XSETWMNAME
#وصف_ڪريو XSetWMNormalHints XSETWMNORMALHINTS
#وصف_ڪريو XSetWMProperties XSETWMPROPERTIES
#وصف_ڪريو XSetWMProtocols XSETWMPROTOCOLS
#وصف_ڪريو XSet_دري_Background XSETWINDOWBACKGROUND
#وصف_ڪريو XSet_دري_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#وصف_ڪريو XSet_دري_Colormap XSETWINDOWCOLORMAP
#وصف_ڪريو XShape_Combine_نقاب XSHAPECOMBINEMASK
#وصف_ڪريو XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#وصف_ڪريو XShapeGetRectangles XSHAPEGETRECTANGLES
#وصف_ڪريو XShrinkRegion XSHRINKREGION
#وصف_ڪريو XStore_بفر XSTOREBUFFER
#وصف_ڪريو XStoreColor XSTORECOLOR
#وصف_ڪريو XStoreColors XSTORECOLORS
#وصف_ڪريو XStoreName XSTORENAME
#وصف_ڪريو XString_منزل_Keysym XSTRINGTOKEYSYM
#وصف_ڪريو XSubtractRegion XSUBTRACTREGION
#وصف_ڪريو XSupportsLocale XSUPPORTSLOCALE
#وصف_ڪريو XSync XSYNC
#وصف_ڪريو XSynchronize XSYNCHRONIZE
#وصف_ڪريو XTextExtents XTEXTEXTENTS
#وصف_ڪريو XTextExtents16 XTEXTEXTENTS16
#وصف_ڪريو XText_ويڪر XTEXTWIDTH
#وصف_ڪريو XTextWidth16 XTEXTWIDTH16
#وصف_ڪريو XTranslateCoordinates XTRANSLATECOORDINATES
#وصف_ڪريو XUndefineCursor XUNDEFINECURSOR
#وصف_ڪريو XUngrabKeyboard XUNGRABKEYBOARD
#وصف_ڪريو XUngrab_اشارو XUNGRABPOINTER
#وصف_ڪريو XUngrabServer XUNGRABSERVER
#وصف_ڪريو XUnionRectWithRegion XUNIONRECTWITHREGION
#وصف_ڪريو XUnionRegion XUNIONREGION
#وصف_ڪريو XUnmap_دري XUNMAPWINDOW
#وصف_ڪريو _XUnregisterFilter _XUNREGISTERFILTER
#وصف_ڪريو XUnsetICFocus XUNSETICFOCUS
#وصف_ڪريو XVaCreateNestedList XVACREATENESTEDLIST
#وصف_ڪريو XVisualIDFromVisual XVISUALIDFROMVISUAL
#وصف_ڪريو XWarp_اشارو XWARPPOINTER
#وصف_ڪريو XWidth_جو_Screen XWIDTHOFSCREEN
#وصف_ڪريو XWindowEvent XWINDOWEVENT
#وصف_ڪريو XWithdraw_دري XWITHDRAWWINDOW
#وصف_ڪريو XXorRegion XXORREGION
#وصف_ڪريو Xm_جوڙيو_Protocol_Callback XMADDPROTOCOLCALLBACK
#وصف_ڪريو Xm_جوڙيو_Protocols XMADDPROTOCOLS
#وصف_ڪريو XmChangeColor XMCHANGECOLOR
#وصف_ڪريو XmClipboardCopy XMCLIPBOARDCOPY
#وصف_ڪريو Xm_Clipboard_پڇاڙي_Copy XMCLIPBOARDENDCOPY
#وصف_ڪريو Xm_Clipboard_Inquire_ڊيگهائي XMCLIPBOARDINQUIRELENGTH
#وصف_ڪريو Xm_Clipboard_تالو XMCLIPBOARDLOCK
#وصف_ڪريو XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#وصف_ڪريو Xm_Clipboard_شروعات_Copy XMCLIPBOARDSTARTCOPY
#وصف_ڪريو XmClipboardUnlock XMCLIPBOARDUNLOCK
#وصف_ڪريو XmCreateArrowButton XMCREATEARROWBUTTON
#وصف_ڪريو XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#وصف_ڪريو XmCreateCascadeButton XMCREATECASCADEBUTTON
#وصف_ڪريو XmCreateDialogShell XMCREATEDIALOGSHELL
#وصف_ڪريو XmCreateDragIcon XMCREATEDRAGICON
#وصف_ڪريو XmCreateDrawingArea XMCREATEDRAWINGAREA
#وصف_ڪريو XmCreateDrawnButton XMCREATEDRAWNBUTTON
#وصف_ڪريو XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#وصف_ڪريو XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#وصف_ڪريو XmCreateForm XMCREATEFORM
#وصف_ڪريو XmCreateFormDialog XMCREATEFORMDIALOG
#وصف_ڪريو Xm_Create_چوکٺو XMCREATEFRAME
#وصف_ڪريو XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#وصف_ڪريو XmCreateLabelGadget XMCREATELABELGADGET
#وصف_ڪريو Xm_Create_Main_دري XMCREATEMAINWINDOW
#وصف_ڪريو XmCreateMenuBar XMCREATEMENUBAR
#وصف_ڪريو Xm_Create_پيغام_Box XMCREATEMESSAGEBOX
#وصف_ڪريو Xm_Create_پيغام_Dialog XMCREATEMESSAGEDIALOG
#وصف_ڪريو XmCreateOptionMenu XMCREATEOPTIONMENU
#وصف_ڪريو Xm_Create_Paned_دري XMCREATEPANEDWINDOW
#وصف_ڪريو XmCreatePopupMenu XMCREATEPOPUPMENU
#وصف_ڪريو XmCreatePromptDialog XMCREATEPROMPTDIALOG
#وصف_ڪريو XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#وصف_ڪريو Xm_Create_ڌڪو_Button XMCREATEPUSHBUTTON
#وصف_ڪريو Xm_Create_ڌڪو_Button_Gadget XMCREATEPUSHBUTTONGADGET
#وصف_ڪريو XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#وصف_ڪريو XmCreateRadioBox XMCREATERADIOBOX
#وصف_ڪريو Xm_Create_صف_ڪالم XMCREATEROWCOLUMN
#وصف_ڪريو XmCreateScale XMCREATESCALE
#وصف_ڪريو XmCreateScrollBar XMCREATESCROLLBAR
#وصف_ڪريو XmCreateScrolledList XMCREATESCROLLEDLIST
#وصف_ڪريو Xm_Create_Scrolled_متن XMCREATESCROLLEDTEXT
#وصف_ڪريو Xm_Create_Scrolled_دري XMCREATESCROLLEDWINDOW
#وصف_ڪريو XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#وصف_ڪريو XmCreateSeparator XMCREATESEPARATOR
#وصف_ڪريو XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#وصف_ڪريو XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#وصف_ڪريو Xm_Create_متن XMCREATETEXT
#وصف_ڪريو Xm_Create_متن_ميدان XMCREATETEXTFIELD
#وصف_ڪريو XmCreateToggleButton XMCREATETOGGLEBUTTON
#وصف_ڪريو XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#وصف_ڪريو Xm_Drag_شروعات XMDRAGSTART
#وصف_ڪريو Xm_Drop_Site_رجسٽر XMDROPSITEREGISTER
#وصف_ڪريو XmDropSiteUnregister XMDROPSITEUNREGISTER
#وصف_ڪريو XmDropSiteUpdate XMDROPSITEUPDATE
#وصف_ڪريو Xm_Drop_Transfer_شروعات XMDROPTRANSFERSTART
#وصف_ڪريو Xm_File_Selection_Box_Get_ٻار XMFILESELECTIONBOXGETCHILD
#وصف_ڪريو Xm_File_Selection_Do_ڳولا XMFILESELECTIONDOSEARCH
#وصف_ڪريو Xm_Font_List_Append_داخلا XMFONTLISTAPPENDENTRY
#وصف_ڪريو XmFontListCopy XMFONTLISTCOPY
#وصف_ڪريو XmFontListCreate XMFONTLISTCREATE
#وصف_ڪريو Xm_Font_List_داخلا_Create XMFONTLISTENTRYCREATE
#وصف_ڪريو Xm_Font_List_داخلا_Free XMFONTLISTENTRYFREE
#وصف_ڪريو Xm_Font_List_داخلا_Get_Font XMFONTLISTENTRYGETFONT
#وصف_ڪريو Xm_Font_List_داخلا_Get_ٽيگ XMFONTLISTENTRYGETTAG
#وصف_ڪريو Xm_Font_List_داخلا_ڀريو XMFONTLISTENTRYLOAD
#وصف_ڪريو XmFontListFree XMFONTLISTFREE
#وصف_ڪريو XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#وصف_ڪريو Xm_Font_List_Get_ايندڙ_Font XMFONTLISTGETNEXTFONT
#وصف_ڪريو XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#وصف_ڪريو Xm_Font_List_ايندڙ_داخلا XMFONTLISTNEXTENTRY
#وصف_ڪريو XmGetColors XMGETCOLORS
#وصف_ڪريو XmGetFocusWidget XMGETFOCUSWIDGET
#وصف_ڪريو XmGetMenuCursor XMGETMENUCURSOR
#وصف_ڪريو Xm_Get_Pixmap_By_اونهائي XMGETPIXMAPBYDEPTH
#وصف_ڪريو XmGetTearOffControl XMGETTEAROFFCONTROL
#وصف_ڪريو XmGetXmDisplay XMGETXMDISPLAY
#وصف_ڪريو XmImMbLookupString XMIMMBLOOKUPSTRING
#وصف_ڪريو Xm_Im_رجسٽر XMIMREGISTER
#وصف_ڪريو Xm_Im_سيٽ_Focus_قدرون XMIMSETFOCUSVALUES
#وصف_ڪريو Xm_Im_سيٽ_قدرون XMIMSETVALUES
#وصف_ڪريو XmImUnregister XMIMUNREGISTER
#وصف_ڪريو XmImUnsetFocus XMIMUNSETFOCUS
#وصف_ڪريو XmInternAtom XMINTERNATOM
#وصف_ڪريو XmIsMotifWMRunning XMISMOTIFWMRUNNING
#وصف_ڪريو Xm_List_جوڙيو_شي XMLISTADDITEM
#وصف_ڪريو Xm_List_جوڙيو_شي_Unselected XMLISTADDITEMUNSELECTED
#وصف_ڪريو Xm_List_جوڙيو_شيون_Unselected XMLISTADDITEMSUNSELECTED
#وصف_ڪريو Xm_List_Delete_All_شيون XMLISTDELETEALLITEMS
#وصف_ڪريو Xm_List_Delete_شيون_Pos XMLISTDELETEITEMSPOS
#وصف_ڪريو XmListDeletePos XMLISTDELETEPOS
#وصف_ڪريو Xm_List_Deselect_All_شيون XMLISTDESELECTALLITEMS
#وصف_ڪريو XmListDeselectPos XMLISTDESELECTPOS
#وصف_ڪريو Xm_List_Get_Kbd_شي_Pos XMLISTGETKBDITEMPOS
#وصف_ڪريو Xm_List_Get_ميلاپ_Pos XMLISTGETMATCHPOS
#وصف_ڪريو XmListGetSelectedPos XMLISTGETSELECTEDPOS
#وصف_ڪريو XmListPosSelected XMLISTPOSSELECTED
#وصف_ڪريو Xm_List_Select_شي XMLISTSELECTITEM
#وصف_ڪريو XmListSelectPos XMLISTSELECTPOS
#وصف_ڪريو Xm_List_سيٽ_Bottom_Pos XMLISTSETBOTTOMPOS
#وصف_ڪريو Xm_List_سيٽ_شي XMLISTSETITEM
#وصف_ڪريو Xm_List_سيٽ_Kbd_شي_Pos XMLISTSETKBDITEMPOS
#وصف_ڪريو Xm_List_سيٽ_Pos XMLISTSETPOS
#وصف_ڪريو Xm_Main_دري_سيٽ_Areas XMMAINWINDOWSETAREAS
#وصف_ڪريو XmMenuPosition XMMENUPOSITION
#وصف_ڪريو Xm_پيغام_Box_Get_ٻار XMMESSAGEBOXGETCHILD
#وصف_ڪريو XmOptionButtonGadget XMOPTIONBUTTONGADGET
#وصف_ڪريو XmOptionLabelGadget XMOPTIONLABELGADGET
#وصف_ڪريو XmProcessTraversal XMPROCESSTRAVERSAL
#وصف_ڪريو XmQmotif XMQMOTIF
#وصف_ڪريو XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#وصف_ڪريو XmRepTypeGetId XMREPTYPEGETID
#وصف_ڪريو Xm_Rep_Type_Get_رڪارڊ XMREPTYPEGETRECORD
#وصف_ڪريو XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#وصف_ڪريو Xm_Rep_Type_رجسٽر XMREPTYPEREGISTER
#وصف_ڪريو Xm_Rep_Type_Valid_قدر XMREPTYPEVALIDVALUE
#وصف_ڪريو Xm_Scroll_Bar_سيٽ_قدرون XMSCROLLBARSETVALUES
#وصف_ڪريو Xm_Scrolled_دري_سيٽ_Areas XMSCROLLEDWINDOWSETAREAS
#وصف_ڪريو Xm_Selection_Box_Get_ٻار XMSELECTIONBOXGETCHILD
#وصف_ڪريو XmStringByteCompare XMSTRINGBYTECOMPARE
#وصف_ڪريو XmStringCompare XMSTRINGCOMPARE
#وصف_ڪريو XmStringConcat XMSTRINGCONCAT
#وصف_ڪريو XmStringCopy XMSTRINGCOPY
#وصف_ڪريو XmStringCreate XMSTRINGCREATE
#وصف_ڪريو XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#وصف_ڪريو XmStringCreateLtoR XMSTRINGCREATELTOR
#وصف_ڪريو XmStringCreateSimple XMSTRINGCREATESIMPLE
#وصف_ڪريو XmStringDraw XMSTRINGDRAW
#وصف_ڪريو XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#وصف_ڪريو XmStringExtent XMSTRINGEXTENT
#وصف_ڪريو XmStringFree XMSTRINGFREE
#وصف_ڪريو XmStringFreeContext XMSTRINGFREECONTEXT
#وصف_ڪريو XmStringGetLtoR XMSTRINGGETLTOR
#وصف_ڪريو Xm_String_Get_ايندڙ_Component XMSTRINGGETNEXTCOMPONENT
#وصف_ڪريو Xm_String_Get_ايندڙ_Segment XMSTRINGGETNEXTSEGMENT
#وصف_ڪريو XmStringInitContext XMSTRINGINITCONTEXT
#وصف_ڪريو Xm_String_ڊيگهائي XMSTRINGLENGTH
#وصف_ڪريو XmStringLtoRCreate XMSTRINGLTORCREATE
#وصف_ڪريو XmStringNConcat XMSTRINGNCONCAT
#وصف_ڪريو XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#وصف_ڪريو Xm_String_ويڪر XMSTRINGWIDTH
#وصف_ڪريو Xm_متن_Clear_Selection XMTEXTCLEARSELECTION
#وصف_ڪريو Xm_متن_ميدان_Get_Editable XMTEXTFIELDGETEDITABLE
#وصف_ڪريو Xm_متن_ميدان_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#وصف_ڪريو Xm_متن_ميدان_Get_آخري_Position XMTEXTFIELDGETLASTPOSITION
#وصف_ڪريو Xm_متن_ميدان_Get_Selection XMTEXTFIELDGETSELECTION
#وصف_ڪريو Xm_متن_ميدان_Get_String XMTEXTFIELDGETSTRING
#وصف_ڪريو Xm_متن_ميدان_وجهو XMTEXTFIELDINSERT
#وصف_ڪريو Xm_متن_ميدان_Remove XMTEXTFIELDREMOVE
#وصف_ڪريو Xm_متن_ميدان_سيٽ_Selection XMTEXTFIELDSETSELECTION
#وصف_ڪريو Xm_متن_ميدان_سيٽ_String XMTEXTFIELDSETSTRING
#وصف_ڪريو Xm_متن_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#وصف_ڪريو Xm_متن_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#وصف_ڪريو Xm_متن_Get_آخري_Position XMTEXTGETLASTPOSITION
#وصف_ڪريو Xm_متن_Get_Max_ڊيگهائي XMTEXTGETMAXLENGTH
#وصف_ڪريو Xm_متن_Get_Selection XMTEXTGETSELECTION
#وصف_ڪريو Xm_متن_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#وصف_ڪريو Xm_متن_Get_String XMTEXTGETSTRING
#وصف_ڪريو Xm_متن_وجهو XMTEXTINSERT
#وصف_ڪريو Xm_متن_Remove XMTEXTREMOVE
#وصف_ڪريو Xm_متن_Replace XMTEXTREPLACE
#وصف_ڪريو Xm_متن_سيٽ_Cursor_Position XMTEXTSETCURSORPOSITION
#وصف_ڪريو Xm_متن_سيٽ_Highlight XMTEXTSETHIGHLIGHT
#وصف_ڪريو Xm_متن_سيٽ_Insertion_Position XMTEXTSETINSERTIONPOSITION
#وصف_ڪريو Xm_متن_سيٽ_Selection XMTEXTSETSELECTION
#وصف_ڪريو Xm_متن_سيٽ_String XMTEXTSETSTRING
#وصف_ڪريو XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#وصف_ڪريو Xm_Toggle_Button_Gadget_سيٽ_State XMTOGGLEBUTTONGADGETSETSTATE
#وصف_ڪريو XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#وصف_ڪريو Xm_Toggle_Button_سيٽ_State XMTOGGLEBUTTONSETSTATE
#وصف_ڪريو XmUpdateDisplay XMUPDATEDISPLAY
#وصف_ڪريو XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#وصف_ڪريو XmbDrawString XMBDRAWSTRING
#وصف_ڪريو XmbLookupString XMBLOOKUPSTRING
#وصف_ڪريو XmbResetIC XMBRESETIC
#وصف_ڪريو Xmb_سيٽ_WMProperties XMBSETWMPROPERTIES
#وصف_ڪريو Xmb_متن_Escapement XMBTEXTESCAPEMENT
#وصف_ڪريو Xmb_متن_Extents XMBTEXTEXTENTS
#وصف_ڪريو Xmb_متن_List_منزل_متن_Property XMBTEXTLISTTOTEXTPROPERTY
#وصف_ڪريو Xmb_متن_Property_منزل_متن_List XMBTEXTPROPERTYTOTEXTLIST
#وصف_ڪريو Xmu_Client_دري XMUCLIENTWINDOW
#وصف_ڪريو Xmu_Print_Default_Error_پيغام XMUPRINTDEFAULTERRORMESSAGE
#وصف_ڪريو XrmGetDatabase XRMGETDATABASE
#وصف_ڪريو XrmGetResource XRMGETRESOURCE
#وصف_ڪريو XrmPutStringResource XRMPUTSTRINGRESOURCE
#وصف_ڪريو Xrm_Quark_منزل_String XRMQUARKTOSTRING
#وصف_ڪريو Xrm_String_منزل_Quark XRMSTRINGTOQUARK
#وصف_ڪريو Xt_جوڙيو_Callback XTADDCALLBACK
#وصف_ڪريو Xt_جوڙيو_Callbacks XTADDCALLBACKS
#وصف_ڪريو Xt_جوڙيو_Event_Handler XTADDEVENTHANDLER
#وصف_ڪريو Xt_جوڙيو_Grab XTADDGRAB
#وصف_ڪريو XtAllocateGC XTALLOCATEGC
#وصف_ڪريو Xt_App_جوڙيو_Actions XTAPPADDACTIONS
#وصف_ڪريو Xt_App_جوڙيو_داخلا XTAPPADDINPUT
#وصف_ڪريو Xt_App_جوڙيو_وقت_ٻاهريون XTAPPADDTIMEOUT
#وصف_ڪريو XtAppCreateShell XTAPPCREATESHELL
#وصف_ڪريو XtAppInitialize XTAPPINITIALIZE
#وصف_ڪريو Xt_App_ايندڙ_Event XTAPPNEXTEVENT
#وصف_ڪريو XtAppPeekEvent XTAPPPEEKEVENT
#وصف_ڪريو XtAppPending XTAPPPENDING
#وصف_ڪريو XtAppProcessEvent XTAPPPROCESSEVENT
#وصف_ڪريو Xt_App_سيٽ_Error_Handler XTAPPSETERRORHANDLER
#وصف_ڪريو Xt_App_سيٽ_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#وصف_ڪريو Xt_App_سيٽ_Warning_Handler XTAPPSETWARNINGHANDLER
#وصف_ڪريو Xt_App_سيٽ_Warning_نياپو_Handler XTAPPSETWARNINGMSGHANDLER
#وصف_ڪريو XtAppWarning XTAPPWARNING
#وصف_ڪريو XtCallActionProc XTCALLACTIONPROC
#وصف_ڪريو XtCallCallbackList XTCALLCALLBACKLIST
#وصف_ڪريو XtCallCallbacks XTCALLCALLBACKS
#وصف_ڪريو XtConfigureWidget XTCONFIGUREWIDGET
#وصف_ڪريو XtConvertAndStore XTCONVERTANDSTORE
#وصف_ڪريو XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#وصف_ڪريو XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#وصف_ڪريو XtCreatePopupShell XTCREATEPOPUPSHELL
#وصف_ڪريو XtCreateWidget XTCREATEWIDGET
#وصف_ڪريو XtDatabase XTDATABASE
#وصف_ڪريو XtDestroyWidget XTDESTROYWIDGET
#وصف_ڪريو XtDisownSelection XTDISOWNSELECTION
#وصف_ڪريو XtDispatchEvent XTDISPATCHEVENT
#وصف_ڪريو Xt_Display_جو_Object XTDISPLAYOFOBJECT
#وصف_ڪريو XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#وصف_ڪريو Xt_Display_منزل_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#وصف_ڪريو XtFree XTFREE
#وصف_ڪريو XtGetActionKeysym XTGETACTIONKEYSYM
#وصف_ڪريو XtGetActionList XTGETACTIONLIST
#وصف_ڪريو XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#وصف_ڪريو XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#وصف_ڪريو XtGetGC XTGETGC
#وصف_ڪريو Xt_Get_Multi_Click_وقت XTGETMULTICLICKTIME
#وصف_ڪريو Xt_Get_Selection_قدر XTGETSELECTIONVALUE
#وصف_ڪريو Xt_Get_Selection_قدرون XTGETSELECTIONVALUES
#وصف_ڪريو XtGetSubresources XTGETSUBRESOURCES
#وصف_ڪريو Xt_Get_قدرون XTGETVALUES
#وصف_ڪريو XtGrabKeyboard XTGRABKEYBOARD
#وصف_ڪريو Xt_Grab_اشارو XTGRABPOINTER
#وصف_ڪريو XtHasCallbacks XTHASCALLBACKS
#وصف_ڪريو XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#وصف_ڪريو Xt_وجهو_Event_Handler XTINSERTEVENTHANDLER
#وصف_ڪريو XtIsManaged XTISMANAGED
#وصف_ڪريو XtIsObject XTISOBJECT
#وصف_ڪريو XtIsSensitive XTISSENSITIVE
#وصف_ڪريو XtIsSubclass XTISSUBCLASS
#وصف_ڪريو Xt_آخري_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#وصف_ڪريو XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#وصف_ڪريو XtMakeResizeRequest XTMAKERESIZEREQUEST
#وصف_ڪريو XtMalloc XTMALLOC
#وصف_ڪريو Xt_Manage_ٻار XTMANAGECHILD
#وصف_ڪريو XtManageChildren XTMANAGECHILDREN
#وصف_ڪريو Xt_ملايو_Arg_Lists XTMERGEARGLISTS
#وصف_ڪريو XtMoveWidget XTMOVEWIDGET
#وصف_ڪريو XtName XTNAME
#وصف_ڪريو Xt_Name_منزل_Widget XTNAMETOWIDGET
#وصف_ڪريو XtOpenDisplay XTOPENDISPLAY
#وصف_ڪريو XtOverrideTranslations XTOVERRIDETRANSLATIONS
#وصف_ڪريو XtOwnSelection XTOWNSELECTION
#وصف_ڪريو Xt_والد XTPARENT
#وصف_ڪريو Xt_Parse_Translation_جدول XTPARSETRANSLATIONTABLE
#وصف_ڪريو XtPopdown XTPOPDOWN
#وصف_ڪريو XtPopup XTPOPUP
#وصف_ڪريو XtQueryGeometry XTQUERYGEOMETRY
#وصف_ڪريو XtRealizeWidget XTREALIZEWIDGET
#وصف_ڪريو XtRealloc XTREALLOC
#وصف_ڪريو XtReleaseGC XTRELEASEGC
#وصف_ڪريو XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#وصف_ڪريو XtRemoveCallback XTREMOVECALLBACK
#وصف_ڪريو XtRemoveEventHandler XTREMOVEEVENTHANDLER
#وصف_ڪريو XtRemoveGrab XTREMOVEGRAB
#وصف_ڪريو Xt_Remove_داخلا XTREMOVEINPUT
#وصف_ڪريو Xt_Remove_وقت_ٻاهريون XTREMOVETIMEOUT
#وصف_ڪريو XtResizeWidget XTRESIZEWIDGET
#وصف_ڪريو XtResolvePathname XTRESOLVEPATHNAME
#وصف_ڪريو Xt_سيٽ_Keyboard_Focus XTSETKEYBOARDFOCUS
#وصف_ڪريو Xt_سيٽ_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#وصف_ڪريو Xt_سيٽ_Sensitive XTSETSENSITIVE
#وصف_ڪريو Xt_سيٽ_Type_Converter XTSETTYPECONVERTER
#وصف_ڪريو Xt_سيٽ_قدرون XTSETVALUES
#وصف_ڪريو Xt_Shell_لڙيون XTSHELLSTRINGS
#وصف_ڪريو Xt_لڙيون XTSTRINGS
#وصف_ڪريو XtToolkitInitialize XTTOOLKITINITIALIZE
#وصف_ڪريو XtTranslateCoords XTTRANSLATECOORDS
#وصف_ڪريو XtTranslateKeycode XTTRANSLATEKEYCODE
#وصف_ڪريو XtUngrabKeyboard XTUNGRABKEYBOARD
#وصف_ڪريو Xt_Ungrab_اشارو XTUNGRABPOINTER
#وصف_ڪريو Xt_Unmanage_ٻار XTUNMANAGECHILD
#وصف_ڪريو XtUnmanageChildren XTUNMANAGECHILDREN
#وصف_ڪريو XtUnrealizeWidget XTUNREALIZEWIDGET
#وصف_ڪريو XtVaAppCreateShell XTVAAPPCREATESHELL
#وصف_ڪريو XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#وصف_ڪريو XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#وصف_ڪريو XtVaCreateWidget XTVACREATEWIDGET
#وصف_ڪريو Xt_Va_Get_قدرون XTVAGETVALUES
#وصف_ڪريو Xt_Va_سيٽ_قدرون XTVASETVALUES
#وصف_ڪريو XtWarning XTWARNING
#وصف_ڪريو Xt_Widget_منزل_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#وصف_ڪريو Xt_دري XTWINDOW
#وصف_ڪريو Xt_دري_جو_Object XTWINDOWOFOBJECT
#وصف_ڪريو Xt_دري_منزل_Widget XTWINDOWTOWIDGET
#وصف_ڪريو XwcDrawString XWCDRAWSTRING
#وصف_ڪريو XwcFreeStringList XWCFREESTRINGLIST
#وصف_ڪريو Xwc_متن_Escapement XWCTEXTESCAPEMENT
#وصف_ڪريو Xwc_متن_Extents XWCTEXTEXTENTS
#وصف_ڪريو Xwc_متن_List_منزل_متن_Property XWCTEXTLISTTOTEXTPROPERTY
#وصف_ڪريو Xwc_متن_Property_منزل_متن_List XWCTEXTPROPERTYTOTEXTLIST
#وصف_ڪريو _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#وصف_ڪريو _XmClearBorder _XMCLEARBORDER
#وصف_ڪريو _XmConfigureObject _XMCONFIGUREOBJECT
#وصف_ڪريو Xm_Destroy_والد_Callback _XMDESTROYPARENTCALLBACK
#وصف_ڪريو _XmDrawArrow _XMDRAWARROW
#وصف_ڪريو _XmDrawShadows _XMDRAWSHADOWS
#وصف_ڪريو _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#وصف_ڪريو _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#وصف_ڪريو _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#وصف_ڪريو _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#وصف_ڪريو _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#وصف_ڪريو _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#وصف_ڪريو Xm_Get_Widget_Ext_معلومات _XMGETWIDGETEXTDATA
#وصف_ڪريو _XmGrabKeyboard _XMGRABKEYBOARD
#وصف_ڪريو Xm_Grab_اشارو _XMGRABPOINTER
#وصف_ڪريو _XmInheritClass _XMINHERITCLASS
#وصف_ڪريو Xm_داخلا_اندريون_Gadget _XMINPUTINGADGET
#وصف_ڪريو _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#وصف_ڪريو Xm_Menu_ڪڍو_هيٺ_ڏانهن _XMMENUPOPDOWN
#وصف_ڪريو _XmMoveObject _XMMOVEOBJECT
#وصف_ڪريو _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#وصف_ڪريو _XmOSBuildFileList _XMOSBUILDFILELIST
#وصف_ڪريو _XmOSFileCompare _XMOSFILECOMPARE
#وصف_ڪريو Xm_OSFind_نمونو_Part _XMOSFINDPATTERNPART
#وصف_ڪريو _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#وصف_ڪريو _XmPostPopupMenu _XMPOSTPOPUPMENU
#وصف_ڪريو _XmPrimitiveEnter _XMPRIMITIVEENTER
#وصف_ڪريو _XmPrimitiveLeave _XMPRIMITIVELEAVE
#وصف_ڪريو _XmRedisplayGadgets _XMREDISPLAYGADGETS
#وصف_ڪريو _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#وصف_ڪريو _XmStringDraw _XMSTRINGDRAW
#وصف_ڪريو Xm_String_Get_متن_Concat _XMSTRINGGETTEXTCONCAT
#وصف_ڪريو Xm_لڙيون _XMSTRINGS
#وصف_ڪريو Xm_منزل_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#وصف_ڪريو Xm_منزل_Vertical_Pixels _XMTOVERTICALPIXELS
#وصف_ڪريو Xm_چوٽي_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#وصف_ڪريو _Xm_fastPtr _XM_FASTPTR
#وصف_ڪريو _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#وصف_ڪريو _XtInherit _XTINHERIT
#وصف_ڪريو _XtInheritTranslations _XTINHERITTRANSLATIONS
#وصف_ڪريو applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#وصف_ڪريو مرکب_Widget_Class COMPOSITEWIDGETCLASS
#وصف_ڪريو overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#وصف_ڪريو shellWidgetClass SHELLWIDGETCLASS
#وصف_ڪريو چوٽي_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#وصف_ڪريو چوٽي_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#وصف_ڪريو transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#وصف_ڪريو vendorShellClassRec VENDORSHELLCLASSREC
#وصف_ڪريو vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#وصف_ڪريو wmShellWidgetClass WMSHELLWIDGETCLASS
#وصف_ڪريو xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#وصف_ڪريو xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#وصف_ڪريو xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#وصف_ڪريو xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#وصف_ڪريو xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#وصف_ڪريو xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#وصف_ڪريو xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#وصف_ڪريو xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#وصف_ڪريو xmFormWidgetClass XMFORMWIDGETCLASS
#وصف_ڪريو xm_چوکٺو_Widget_Class XMFRAMEWIDGETCLASS
#وصف_ڪريو xmGadgetClass XMGADGETCLASS
#وصف_ڪريو xmLabelGadgetClass XMLABELGADGETCLASS
#وصف_ڪريو xmLabelWidgetClass XMLABELWIDGETCLASS
#وصف_ڪريو xmListWidgetClass XMLISTWIDGETCLASS
#وصف_ڪريو xm_Main_دري_Widget_Class XMMAINWINDOWWIDGETCLASS
#وصف_ڪريو xmManagerClassRec XMMANAGERCLASSREC
#وصف_ڪريو xmManagerWidgetClass XMMANAGERWIDGETCLASS
#وصف_ڪريو xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#وصف_ڪريو xm_پيغام_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#وصف_ڪريو xmPrimitiveClassRec XMPRIMITIVECLASSREC
#وصف_ڪريو xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#وصف_ڪريو xm_ڌڪو_Button_Class_Rec XMPUSHBUTTONCLASSREC
#وصف_ڪريو xm_ڌڪو_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#وصف_ڪريو xm_ڌڪو_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#وصف_ڪريو xm_صف_ڪالم_Widget_Class XMROWCOLUMNWIDGETCLASS
#وصف_ڪريو xmSashWidgetClass XMSASHWIDGETCLASS
#وصف_ڪريو xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#وصف_ڪريو xm_Scrolled_دري_Class_Rec XMSCROLLEDWINDOWCLASSREC
#وصف_ڪريو xm_Scrolled_دري_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#وصف_ڪريو xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#وصف_ڪريو xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#وصف_ڪريو xm_متن_ميدان_Widget_Class XMTEXTFIELDWIDGETCLASS
#وصف_ڪريو xm_متن_Widget_Class XMTEXTWIDGETCLASS
#وصف_ڪريو xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#وصف_ڪريو xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#جيڪڏهن_وصف_نه_ٿيل VMS_X11R6
// R5 or earlier
#وصف_ڪريو Xt_رجسٽر_دري _XTREGISTERWINDOW
#وصف_ڪريو Xt_Unregister_دري _XTUNREGISTERWINDOW
#وإلا
// R6 or later
#وصف_ڪريو Xt_رجسٽر_Drawable XTREGISTERDRAWABLE
#وصف_ڪريو XtUnregisterDrawable XTUNREGISTERDRAWABLE
#جيڪڏهن_پورو
