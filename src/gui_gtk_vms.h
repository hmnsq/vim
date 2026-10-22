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

#تعریف_کرو _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#تعریف_کرو XAllocClassHint XALLOCCLASSHINT
#تعریف_کرو XAllocColor XALLOCCOLOR
#تعریف_کرو XAllocColorCells XALLOCCOLORCELLS
#تعریف_کرو XAlloc_ناپ_Hints XALLOCSIZEHINTS
#تعریف_کرو XAllocWMHints XALLOCWMHINTS
#تعریف_کرو XAutoRepeatOff XAUTOREPEATOFF
#تعریف_کرو XAutoRepeatOn XAUTOREPEATON
#تعریف_کرو XBase_Font_Name_List_دا_Font_سیٹ XBASEFONTNAMELISTOFFONTSET
#تعریف_کرو XBell XBELL
#تعریف_کرو XBitmapPad XBITMAPPAD
#تعریف_کرو XChange_سرگرم_اشارہ_گر_Grab XCHANGEACTIVEPOINTERGRAB
#تعریف_کرو XChangeGC XCHANGEGC
#تعریف_کرو XChangeProperty XCHANGEPROPERTY
#تعریف_کرو XChange_کھڑکی_گن XCHANGEWINDOWATTRIBUTES
#تعریف_کرو XCheckIfEvent XCHECKIFEVENT
#تعریف_کرو XCheck_نقاب_Event XCHECKMASKEVENT
#تعریف_کرو XCheckTypedEvent XCHECKTYPEDEVENT
#تعریف_کرو XCheck_Typed_کھڑکی_Event XCHECKTYPEDWINDOWEVENT
#تعریف_کرو XCheck_کھڑکی_Event XCHECKWINDOWEVENT
#تعریف_کرو XClearArea XCLEARAREA
#تعریف_کرو XClear_کھڑکی XCLEARWINDOW
#تعریف_کرو XClipBox XCLIPBOX
#تعریف_کرو XCloseDisplay XCLOSEDISPLAY
#تعریف_کرو XCloseIM XCLOSEIM
#تعریف_کرو XConfigure_کھڑکی XCONFIGUREWINDOW
#تعریف_کرو XConvertSelection XCONVERTSELECTION
#تعریف_کرو XCopyArea XCOPYAREA
#تعریف_کرو XCopyGC XCOPYGC
#تعریف_کرو XCopyPlane XCOPYPLANE
#تعریف_کرو XCreate_Bitmap_From_انکڑے XCREATEBITMAPFROMDATA
#تعریف_کرو XCreateColormap XCREATECOLORMAP
#تعریف_کرو XCreateFontCursor XCREATEFONTCURSOR
#تعریف_کرو XCreate_Font_سیٹ XCREATEFONTSET
#تعریف_کرو XCreateGC XCREATEGC
#تعریف_کرو XCreateIC XCREATEIC
#تعریف_کرو XCreateImage XCREATEIMAGE
#تعریف_کرو XCreatePixmap XCREATEPIXMAP
#تعریف_کرو XCreatePixmapCursor XCREATEPIXMAPCURSOR
#تعریف_کرو XCreate_Pixmap_From_Bitmap_انکڑے XCREATEPIXMAPFROMBITMAPDATA
#تعریف_کرو XCreateRegion XCREATEREGION
#تعریف_کرو XCreate_Simple_کھڑکی XCREATESIMPLEWINDOW
#تعریف_کرو XCreate_کھڑکی XCREATEWINDOW
#تعریف_کرو XDefault_جڑ_کھڑکی XDEFAULTROOTWINDOW
#تعریف_کرو XDefault_Screen_دا_Display XDEFAULTSCREENOFDISPLAY
#تعریف_کرو XDefineCursor XDEFINECURSOR
#تعریف_کرو XDeleteProperty XDELETEPROPERTY
#تعریف_کرو XDestroyIC XDESTROYIC
#تعریف_کرو XDestroyRegion XDESTROYREGION
#تعریف_کرو XDestroy_کھڑکی XDESTROYWINDOW
#تعریف_کرو XDisplayName XDISPLAYNAME
#تعریف_کرو XDisplay_دا_Screen XDISPLAYOFSCREEN
#تعریف_کرو XDisplayString XDISPLAYSTRING
#تعریف_کرو XDrawArc XDRAWARC
#تعریف_کرو XDrawImageString XDRAWIMAGESTRING
#تعریف_کرو XDrawImageString16 XDRAWIMAGESTRING16
#تعریف_کرو XDraw_سطر XDRAWLINE
#تعریف_کرو XDraw_سطریں XDRAWLINES
#تعریف_کرو XDraw_نقطہ XDRAWPOINT
#تعریف_کرو XDrawPoints XDRAWPOINTS
#تعریف_کرو XDrawRectangle XDRAWRECTANGLE
#تعریف_کرو XDrawSegments XDRAWSEGMENTS
#تعریف_کرو XDrawString XDRAWSTRING
#تعریف_کرو XDrawString16 XDRAWSTRING16
#تعریف_کرو XEmptyRegion XEMPTYREGION
#تعریف_کرو XEqualRegion XEQUALREGION
#تعریف_کرو XEventsQueued XEVENTSQUEUED
#تعریف_کرو XExtents_دا_Font_سیٹ XEXTENTSOFFONTSET
#تعریف_کرو XFetch_بفر XFETCHBUFFER
#تعریف_کرو XFillArc XFILLARC
#تعریف_کرو XFillPolygon XFILLPOLYGON
#تعریف_کرو XFillRectangle XFILLRECTANGLE
#تعریف_کرو XFillRectangles XFILLRECTANGLES
#تعریف_کرو XFilterEvent XFILTEREVENT
#تعریف_کرو XFlush XFLUSH
#تعریف_کرو XFonts_دا_Font_سیٹ XFONTSOFFONTSET
#تعریف_کرو XFree XFREE
#تعریف_کرو XFreeColormap XFREECOLORMAP
#تعریف_کرو XFreeColors XFREECOLORS
#تعریف_کرو XFreeCursor XFREECURSOR
#تعریف_کرو XFreeFont XFREEFONT
#تعریف_کرو XFreeFontInfo XFREEFONTINFO
#تعریف_کرو XFree_Font_ناں XFREEFONTNAMES
#تعریف_کرو XFree_Font_سیٹ XFREEFONTSET
#تعریف_کرو XFreeGC XFREEGC
#تعریف_کرو XFreeModifiermap XFREEMODIFIERMAP
#تعریف_کرو XFreePixmap XFREEPIXMAP
#تعریف_کرو XFreeStringList XFREESTRINGLIST
#تعریف_کرو XGetAtomName XGETATOMNAME
#تعریف_کرو XGetDefault XGETDEFAULT
#تعریف_کرو XGet_Error_Database_متن XGETERRORDATABASETEXT
#تعریف_کرو XGet_Error_متن XGETERRORTEXT
#تعریف_کرو XGetFontProperty XGETFONTPROPERTY
#تعریف_کرو XGetGCValues XGETGCVALUES
#تعریف_کرو XGetGeometry XGETGEOMETRY
#تعریف_کرو XGetIconSizes XGETICONSIZES
#تعریف_کرو XGetICValues XGETICVALUES
#تعریف_کرو XGetIMValues XGETIMVALUES
#تعریف_کرو XGetImage XGETIMAGE
#تعریف_کرو XGetKeyboardControl XGETKEYBOARDCONTROL
#تعریف_کرو XGetModifierMapping XGETMODIFIERMAPPING
#تعریف_کرو XGetMotionEvents XGETMOTIONEVENTS
#تعریف_کرو XGet_عام_Hints XGETNORMALHINTS
#تعریف_کرو XGetSelectionOwner XGETSELECTIONOWNER
#تعریف_کرو XGetSubImage XGETSUBIMAGE
#تعریف_کرو XGetVisualInfo XGETVISUALINFO
#تعریف_کرو XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#تعریف_کرو XGetWMIconName XGETWMICONNAME
#تعریف_کرو XGetWMProtocols XGETWMPROTOCOLS
#تعریف_کرو XGetWMHints XGETWMHINTS
#تعریف_کرو XGetWMName XGETWMNAME
#تعریف_کرو XGetWMNormalHints XGETWMNORMALHINTS
#تعریف_کرو XGet_کھڑکی_گن XGETWINDOWATTRIBUTES
#تعریف_کرو XGet_کھڑکی_Property XGETWINDOWPROPERTY
#تعریف_کرو XGrabKeyboard XGRABKEYBOARD
#تعریف_کرو XGrab_اشارہ_گر XGRABPOINTER
#تعریف_کرو XGrabServer XGRABSERVER
#تعریف_کرو XHeight_دا_Screen XHEIGHTOFSCREEN
#تعریف_کرو XIconify_کھڑکی XICONIFYWINDOW
#تعریف_کرو XIfEvent XIFEVENT
#تعریف_کرو XInternAtom XINTERNATOM
#تعریف_کرو XIntersectRegion XINTERSECTREGION
#تعریف_کرو XKeycode_منزل_Keysym XKEYCODETOKEYSYM
#تعریف_کرو XKeysym_منزل_Keycode XKEYSYMTOKEYCODE
#تعریف_کرو XKeysym_منزل_String XKEYSYMTOSTRING
#تعریف_کرو XListFonts XLISTFONTS
#تعریف_کرو XListFontsWithInfo XLISTFONTSWITHINFO
#تعریف_کرو XListPixmapFormats XLISTPIXMAPFORMATS
#تعریف_کرو XListProperties XLISTPROPERTIES
#تعریف_کرو XLoadQueryFont XLOADQUERYFONT
#تعریف_کرو XLookupString XLOOKUPSTRING
#تعریف_کرو XLower_کھڑکی XLOWERWINDOW
#تعریف_کرو XMapRaised XMAPRAISED
#تعریف_کرو XMap_کھڑکی XMAPWINDOW
#تعریف_کرو XMatchVisualInfo XMATCHVISUALINFO
#تعریف_کرو XMove_Resize_کھڑکی XMOVERESIZEWINDOW
#تعریف_کرو XMove_کھڑکی XMOVEWINDOW
#تعریف_کرو XNextEvent XNEXTEVENT
#تعریف_کرو XOffsetRegion XOFFSETREGION
#تعریف_کرو XOpenDisplay XOPENDISPLAY
#تعریف_کرو XOpenIM XOPENIM
#تعریف_کرو XParseColor XPARSECOLOR
#تعریف_کرو XParseGeometry XPARSEGEOMETRY
#تعریف_کرو XPeekEvent XPEEKEVENT
#تعریف_کرو XPending XPENDING
#تعریف_کرو XPoint_وچ_Region XPOINTINREGION
#تعریف_کرو XPolygonRegion XPOLYGONREGION
#تعریف_کرو XPut_پٹھ_Event XPUTBACKEVENT
#تعریف_کرو XPutImage XPUTIMAGE
#تعریف_کرو XRoot_کھڑکی XROOTWINDOW
#تعریف_کرو XQueryColor XQUERYCOLOR
#تعریف_کرو XQueryColors XQUERYCOLORS
#تعریف_کرو XQueryExtension XQUERYEXTENSION
#تعریف_کرو XQuery_اشارہ_گر XQUERYPOINTER
#تعریف_کرو XQuery_درخت XQUERYTREE
#تعریف_کرو XRaise_کھڑکی XRAISEWINDOW
#تعریف_کرو XReconfigureWMWindow XRECONFIGUREWMWINDOW
#تعریف_کرو XRect_وچ_Region XRECTINREGION
#تعریف_کرو XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#تعریف_کرو XReparent_کھڑکی XREPARENTWINDOW
#تعریف_کرو XResize_کھڑکی XRESIZEWINDOW
#تعریف_کرو XRestackWindows XRESTACKWINDOWS
#تعریف_کرو XRoot_کھڑکی_دا_Screen XROOTWINDOWOFSCREEN
#تعریف_کرو XScreen_نمبر_دا_Screen XSCREENNUMBEROFSCREEN
#تعریف_کرو XSelectAsyncEvent XSELECTASYNCEVENT
#تعریف_کرو XSelect_Async_داخلہ XSELECTASYNCINPUT
#تعریف_کرو XSelect_داخلہ XSELECTINPUT
#تعریف_کرو XSendEvent XSENDEVENT
#تعریف_کرو XServerVendor XSERVERVENDOR
#تعریف_کرو XSetBackground XSETBACKGROUND
#تعریف_کرو XSetClassHint XSETCLASSHINT
#تعریف_کرو XSet_Clip_نقاب XSETCLIPMASK
#تعریف_کرو XSetClipOrigin XSETCLIPORIGIN
#تعریف_کرو XSetClipRectangles XSETCLIPRECTANGLES
#تعریف_کرو XSet_Close_تھلے_منوال XSETCLOSEDOWNMODE
#تعریف_کرو XSetCommand XSETCOMMAND
#تعریف_کرو XSetDashes XSETDASHES
#تعریف_کرو XSetErrorHandler XSETERRORHANDLER
#تعریف_کرو XSetFillStyle XSETFILLSTYLE
#تعریف_کرو XSetFont XSETFONT
#تعریف_کرو XSetForeground XSETFOREGROUND
#تعریف_کرو XSetFunction XSETFUNCTION
#تعریف_کرو XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#تعریف_کرو XSetICFocus XSETICFOCUS
#تعریف_کرو XSetICValues XSETICVALUES
#تعریف_کرو XSetIOErrorHandler XSETIOERRORHANDLER
#تعریف_کرو XSet_داخلہ_Focus XSETINPUTFOCUS
#تعریف_کرو XSet_سطر_گن XSETLINEATTRIBUTES
#تعریف_کرو XSetLocaleModifiers XSETLOCALEMODIFIERS
#تعریف_کرو XSet_عام_Hints XSETNORMALHINTS
#تعریف_کرو XSetRegion XSETREGION
#تعریف_کرو XSetSelectionOwner XSETSELECTIONOWNER
#تعریف_کرو XSetStipple XSETSTIPPLE
#تعریف_کرو XSet_Subwindow_منوال XSETSUBWINDOWMODE
#تعریف_کرو XSetTSOrigin XSETTSORIGIN
#تعریف_کرو XSetTile XSETTILE
#تعریف_کرو XSetTransientForHint XSETTRANSIENTFORHINT
#تعریف_کرو XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#تعریف_کرو XSetWMHints XSETWMHINTS
#تعریف_کرو XSetWMIconName XSETWMICONNAME
#تعریف_کرو XSetWMName XSETWMNAME
#تعریف_کرو XSetWMNormalHints XSETWMNORMALHINTS
#تعریف_کرو XSetWMProperties XSETWMPROPERTIES
#تعریف_کرو XSetWMProtocols XSETWMPROTOCOLS
#تعریف_کرو XSet_کھڑکی_Background XSETWINDOWBACKGROUND
#تعریف_کرو XSet_کھڑکی_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#تعریف_کرو XSet_کھڑکی_Colormap XSETWINDOWCOLORMAP
#تعریف_کرو XShape_Combine_نقاب XSHAPECOMBINEMASK
#تعریف_کرو XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#تعریف_کرو XShapeGetRectangles XSHAPEGETRECTANGLES
#تعریف_کرو XShrinkRegion XSHRINKREGION
#تعریف_کرو XStore_بفر XSTOREBUFFER
#تعریف_کرو XStoreColor XSTORECOLOR
#تعریف_کرو XStoreColors XSTORECOLORS
#تعریف_کرو XStoreName XSTORENAME
#تعریف_کرو XString_منزل_Keysym XSTRINGTOKEYSYM
#تعریف_کرو XSubtractRegion XSUBTRACTREGION
#تعریف_کرو XSupportsLocale XSUPPORTSLOCALE
#تعریف_کرو XSync XSYNC
#تعریف_کرو XSynchronize XSYNCHRONIZE
#تعریف_کرو XTextExtents XTEXTEXTENTS
#تعریف_کرو XTextExtents16 XTEXTEXTENTS16
#تعریف_کرو XText_چوڑائی XTEXTWIDTH
#تعریف_کرو XTextWidth16 XTEXTWIDTH16
#تعریف_کرو XTranslateCoordinates XTRANSLATECOORDINATES
#تعریف_کرو XUndefineCursor XUNDEFINECURSOR
#تعریف_کرو XUngrabKeyboard XUNGRABKEYBOARD
#تعریف_کرو XUngrab_اشارہ_گر XUNGRABPOINTER
#تعریف_کرو XUngrabServer XUNGRABSERVER
#تعریف_کرو XUnionRectWithRegion XUNIONRECTWITHREGION
#تعریف_کرو XUnionRegion XUNIONREGION
#تعریف_کرو XUnmap_کھڑکی XUNMAPWINDOW
#تعریف_کرو _XUnregisterFilter _XUNREGISTERFILTER
#تعریف_کرو XUnsetICFocus XUNSETICFOCUS
#تعریف_کرو XVaCreateNestedList XVACREATENESTEDLIST
#تعریف_کرو XVisualIDFromVisual XVISUALIDFROMVISUAL
#تعریف_کرو XWarp_اشارہ_گر XWARPPOINTER
#تعریف_کرو XWidth_دا_Screen XWIDTHOFSCREEN
#تعریف_کرو XWindowEvent XWINDOWEVENT
#تعریف_کرو XWithdraw_کھڑکی XWITHDRAWWINDOW
#تعریف_کرو XXorRegion XXORREGION
#تعریف_کرو Xm_جوڑو_Protocol_Callback XMADDPROTOCOLCALLBACK
#تعریف_کرو Xm_جوڑو_Protocols XMADDPROTOCOLS
#تعریف_کرو XmChangeColor XMCHANGECOLOR
#تعریف_کرو XmClipboardCopy XMCLIPBOARDCOPY
#تعریف_کرو Xm_Clipboard_سرا_Copy XMCLIPBOARDENDCOPY
#تعریف_کرو Xm_Clipboard_Inquire_لمائی XMCLIPBOARDINQUIRELENGTH
#تعریف_کرو Xm_Clipboard_جندرا XMCLIPBOARDLOCK
#تعریف_کرو XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#تعریف_کرو Xm_Clipboard_شروع_Copy XMCLIPBOARDSTARTCOPY
#تعریف_کرو XmClipboardUnlock XMCLIPBOARDUNLOCK
#تعریف_کرو XmCreateArrowButton XMCREATEARROWBUTTON
#تعریف_کرو XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#تعریف_کرو XmCreateCascadeButton XMCREATECASCADEBUTTON
#تعریف_کرو XmCreateDialogShell XMCREATEDIALOGSHELL
#تعریف_کرو XmCreateDragIcon XMCREATEDRAGICON
#تعریف_کرو XmCreateDrawingArea XMCREATEDRAWINGAREA
#تعریف_کرو XmCreateDrawnButton XMCREATEDRAWNBUTTON
#تعریف_کرو XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#تعریف_کرو XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#تعریف_کرو XmCreateForm XMCREATEFORM
#تعریف_کرو XmCreateFormDialog XMCREATEFORMDIALOG
#تعریف_کرو Xm_Create_چوکھٹا XMCREATEFRAME
#تعریف_کرو XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#تعریف_کرو XmCreateLabelGadget XMCREATELABELGADGET
#تعریف_کرو Xm_Create_Main_کھڑکی XMCREATEMAINWINDOW
#تعریف_کرو XmCreateMenuBar XMCREATEMENUBAR
#تعریف_کرو Xm_Create_سنیہا_Box XMCREATEMESSAGEBOX
#تعریف_کرو Xm_Create_سنیہا_Dialog XMCREATEMESSAGEDIALOG
#تعریف_کرو XmCreateOptionMenu XMCREATEOPTIONMENU
#تعریف_کرو Xm_Create_Paned_کھڑکی XMCREATEPANEDWINDOW
#تعریف_کرو XmCreatePopupMenu XMCREATEPOPUPMENU
#تعریف_کرو XmCreatePromptDialog XMCREATEPROMPTDIALOG
#تعریف_کرو XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#تعریف_کرو Xm_Create_ڈالو_Button XMCREATEPUSHBUTTON
#تعریف_کرو Xm_Create_ڈالو_Button_Gadget XMCREATEPUSHBUTTONGADGET
#تعریف_کرو XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#تعریف_کرو XmCreateRadioBox XMCREATERADIOBOX
#تعریف_کرو Xm_Create_صف_کالم XMCREATEROWCOLUMN
#تعریف_کرو XmCreateScale XMCREATESCALE
#تعریف_کرو XmCreateScrollBar XMCREATESCROLLBAR
#تعریف_کرو XmCreateScrolledList XMCREATESCROLLEDLIST
#تعریف_کرو Xm_Create_Scrolled_متن XMCREATESCROLLEDTEXT
#تعریف_کرو Xm_Create_Scrolled_کھڑکی XMCREATESCROLLEDWINDOW
#تعریف_کرو XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#تعریف_کرو XmCreateSeparator XMCREATESEPARATOR
#تعریف_کرو XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#تعریف_کرو XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#تعریف_کرو Xm_Create_متن XMCREATETEXT
#تعریف_کرو Xm_Create_متن_میدان XMCREATETEXTFIELD
#تعریف_کرو XmCreateToggleButton XMCREATETOGGLEBUTTON
#تعریف_کرو XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#تعریف_کرو Xm_Drag_شروع XMDRAGSTART
#تعریف_کرو Xm_Drop_Site_رجسٹر XMDROPSITEREGISTER
#تعریف_کرو XmDropSiteUnregister XMDROPSITEUNREGISTER
#تعریف_کرو XmDropSiteUpdate XMDROPSITEUPDATE
#تعریف_کرو Xm_Drop_Transfer_شروع XMDROPTRANSFERSTART
#تعریف_کرو Xm_File_Selection_Box_Get_بچہ XMFILESELECTIONBOXGETCHILD
#تعریف_کرو Xm_File_Selection_Do_تلاش XMFILESELECTIONDOSEARCH
#تعریف_کرو Xm_Font_List_Append_اندراج XMFONTLISTAPPENDENTRY
#تعریف_کرو XmFontListCopy XMFONTLISTCOPY
#تعریف_کرو XmFontListCreate XMFONTLISTCREATE
#تعریف_کرو Xm_Font_List_اندراج_Create XMFONTLISTENTRYCREATE
#تعریف_کرو Xm_Font_List_اندراج_Free XMFONTLISTENTRYFREE
#تعریف_کرو Xm_Font_List_اندراج_Get_Font XMFONTLISTENTRYGETFONT
#تعریف_کرو Xm_Font_List_اندراج_Get_ٹیگ XMFONTLISTENTRYGETTAG
#تعریف_کرو Xm_Font_List_اندراج_لدو XMFONTLISTENTRYLOAD
#تعریف_کرو XmFontListFree XMFONTLISTFREE
#تعریف_کرو XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#تعریف_کرو Xm_Font_List_Get_اگلا_Font XMFONTLISTGETNEXTFONT
#تعریف_کرو XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#تعریف_کرو Xm_Font_List_اگلا_اندراج XMFONTLISTNEXTENTRY
#تعریف_کرو XmGetColors XMGETCOLORS
#تعریف_کرو XmGetFocusWidget XMGETFOCUSWIDGET
#تعریف_کرو XmGetMenuCursor XMGETMENUCURSOR
#تعریف_کرو Xm_Get_Pixmap_By_ڈونگھائی XMGETPIXMAPBYDEPTH
#تعریف_کرو XmGetTearOffControl XMGETTEAROFFCONTROL
#تعریف_کرو XmGetXmDisplay XMGETXMDISPLAY
#تعریف_کرو XmImMbLookupString XMIMMBLOOKUPSTRING
#تعریف_کرو Xm_Im_رجسٹر XMIMREGISTER
#تعریف_کرو Xm_Im_سیٹ_Focus_اقدار XMIMSETFOCUSVALUES
#تعریف_کرو Xm_Im_سیٹ_اقدار XMIMSETVALUES
#تعریف_کرو XmImUnregister XMIMUNREGISTER
#تعریف_کرو XmImUnsetFocus XMIMUNSETFOCUS
#تعریف_کرو XmInternAtom XMINTERNATOM
#تعریف_کرو XmIsMotifWMRunning XMISMOTIFWMRUNNING
#تعریف_کرو Xm_List_جوڑو_شے XMLISTADDITEM
#تعریف_کرو Xm_List_جوڑو_شے_Unselected XMLISTADDITEMUNSELECTED
#تعریف_کرو Xm_List_جوڑو_اشیاء_Unselected XMLISTADDITEMSUNSELECTED
#تعریف_کرو Xm_List_Delete_All_اشیاء XMLISTDELETEALLITEMS
#تعریف_کرو Xm_List_Delete_اشیاء_Pos XMLISTDELETEITEMSPOS
#تعریف_کرو XmListDeletePos XMLISTDELETEPOS
#تعریف_کرو Xm_List_Deselect_All_اشیاء XMLISTDESELECTALLITEMS
#تعریف_کرو XmListDeselectPos XMLISTDESELECTPOS
#تعریف_کرو Xm_List_Get_Kbd_شے_Pos XMLISTGETKBDITEMPOS
#تعریف_کرو Xm_List_Get_میل_Pos XMLISTGETMATCHPOS
#تعریف_کرو XmListGetSelectedPos XMLISTGETSELECTEDPOS
#تعریف_کرو XmListPosSelected XMLISTPOSSELECTED
#تعریف_کرو Xm_List_Select_شے XMLISTSELECTITEM
#تعریف_کرو XmListSelectPos XMLISTSELECTPOS
#تعریف_کرو Xm_List_سیٹ_Bottom_Pos XMLISTSETBOTTOMPOS
#تعریف_کرو Xm_List_سیٹ_شے XMLISTSETITEM
#تعریف_کرو Xm_List_سیٹ_Kbd_شے_Pos XMLISTSETKBDITEMPOS
#تعریف_کرو Xm_List_سیٹ_Pos XMLISTSETPOS
#تعریف_کرو Xm_Main_کھڑکی_سیٹ_Areas XMMAINWINDOWSETAREAS
#تعریف_کرو XmMenuPosition XMMENUPOSITION
#تعریف_کرو Xm_سنیہا_Box_Get_بچہ XMMESSAGEBOXGETCHILD
#تعریف_کرو XmOptionButtonGadget XMOPTIONBUTTONGADGET
#تعریف_کرو XmOptionLabelGadget XMOPTIONLABELGADGET
#تعریف_کرو XmProcessTraversal XMPROCESSTRAVERSAL
#تعریف_کرو XmQmotif XMQMOTIF
#تعریف_کرو XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#تعریف_کرو XmRepTypeGetId XMREPTYPEGETID
#تعریف_کرو Xm_Rep_Type_Get_ریکارڈ XMREPTYPEGETRECORD
#تعریف_کرو XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#تعریف_کرو Xm_Rep_Type_رجسٹر XMREPTYPEREGISTER
#تعریف_کرو Xm_Rep_Type_Valid_قدر XMREPTYPEVALIDVALUE
#تعریف_کرو Xm_Scroll_Bar_سیٹ_اقدار XMSCROLLBARSETVALUES
#تعریف_کرو Xm_Scrolled_کھڑکی_سیٹ_Areas XMSCROLLEDWINDOWSETAREAS
#تعریف_کرو Xm_Selection_Box_Get_بچہ XMSELECTIONBOXGETCHILD
#تعریف_کرو XmStringByteCompare XMSTRINGBYTECOMPARE
#تعریف_کرو XmStringCompare XMSTRINGCOMPARE
#تعریف_کرو XmStringConcat XMSTRINGCONCAT
#تعریف_کرو XmStringCopy XMSTRINGCOPY
#تعریف_کرو XmStringCreate XMSTRINGCREATE
#تعریف_کرو XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#تعریف_کرو XmStringCreateLtoR XMSTRINGCREATELTOR
#تعریف_کرو XmStringCreateSimple XMSTRINGCREATESIMPLE
#تعریف_کرو XmStringDraw XMSTRINGDRAW
#تعریف_کرو XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#تعریف_کرو XmStringExtent XMSTRINGEXTENT
#تعریف_کرو XmStringFree XMSTRINGFREE
#تعریف_کرو XmStringFreeContext XMSTRINGFREECONTEXT
#تعریف_کرو XmStringGetLtoR XMSTRINGGETLTOR
#تعریف_کرو Xm_String_Get_اگلا_Component XMSTRINGGETNEXTCOMPONENT
#تعریف_کرو Xm_String_Get_اگلا_Segment XMSTRINGGETNEXTSEGMENT
#تعریف_کرو XmStringInitContext XMSTRINGINITCONTEXT
#تعریف_کرو Xm_String_لمائی XMSTRINGLENGTH
#تعریف_کرو XmStringLtoRCreate XMSTRINGLTORCREATE
#تعریف_کرو XmStringNConcat XMSTRINGNCONCAT
#تعریف_کرو XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#تعریف_کرو Xm_String_چوڑائی XMSTRINGWIDTH
#تعریف_کرو Xm_متن_Clear_Selection XMTEXTCLEARSELECTION
#تعریف_کرو Xm_متن_میدان_Get_Editable XMTEXTFIELDGETEDITABLE
#تعریف_کرو Xm_متن_میدان_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#تعریف_کرو Xm_متن_میدان_Get_آخری_Position XMTEXTFIELDGETLASTPOSITION
#تعریف_کرو Xm_متن_میدان_Get_Selection XMTEXTFIELDGETSELECTION
#تعریف_کرو Xm_متن_میدان_Get_String XMTEXTFIELDGETSTRING
#تعریف_کرو Xm_متن_میدان_داخل_کرو XMTEXTFIELDINSERT
#تعریف_کرو Xm_متن_میدان_Remove XMTEXTFIELDREMOVE
#تعریف_کرو Xm_متن_میدان_سیٹ_Selection XMTEXTFIELDSETSELECTION
#تعریف_کرو Xm_متن_میدان_سیٹ_String XMTEXTFIELDSETSTRING
#تعریف_کرو Xm_متن_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#تعریف_کرو Xm_متن_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#تعریف_کرو Xm_متن_Get_آخری_Position XMTEXTGETLASTPOSITION
#تعریف_کرو Xm_متن_Get_Max_لمائی XMTEXTGETMAXLENGTH
#تعریف_کرو Xm_متن_Get_Selection XMTEXTGETSELECTION
#تعریف_کرو Xm_متن_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#تعریف_کرو Xm_متن_Get_String XMTEXTGETSTRING
#تعریف_کرو Xm_متن_داخل_کرو XMTEXTINSERT
#تعریف_کرو Xm_متن_Remove XMTEXTREMOVE
#تعریف_کرو Xm_متن_Replace XMTEXTREPLACE
#تعریف_کرو Xm_متن_سیٹ_Cursor_Position XMTEXTSETCURSORPOSITION
#تعریف_کرو Xm_متن_سیٹ_Highlight XMTEXTSETHIGHLIGHT
#تعریف_کرو Xm_متن_سیٹ_Insertion_Position XMTEXTSETINSERTIONPOSITION
#تعریف_کرو Xm_متن_سیٹ_Selection XMTEXTSETSELECTION
#تعریف_کرو Xm_متن_سیٹ_String XMTEXTSETSTRING
#تعریف_کرو XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#تعریف_کرو Xm_Toggle_Button_Gadget_سیٹ_State XMTOGGLEBUTTONGADGETSETSTATE
#تعریف_کرو XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#تعریف_کرو Xm_Toggle_Button_سیٹ_State XMTOGGLEBUTTONSETSTATE
#تعریف_کرو XmUpdateDisplay XMUPDATEDISPLAY
#تعریف_کرو XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#تعریف_کرو XmbDrawString XMBDRAWSTRING
#تعریف_کرو XmbLookupString XMBLOOKUPSTRING
#تعریف_کرو XmbResetIC XMBRESETIC
#تعریف_کرو Xmb_سیٹ_WMProperties XMBSETWMPROPERTIES
#تعریف_کرو Xmb_متن_Escapement XMBTEXTESCAPEMENT
#تعریف_کرو Xmb_متن_Extents XMBTEXTEXTENTS
#تعریف_کرو Xmb_متن_List_منزل_متن_Property XMBTEXTLISTTOTEXTPROPERTY
#تعریف_کرو Xmb_متن_Property_منزل_متن_List XMBTEXTPROPERTYTOTEXTLIST
#تعریف_کرو Xmu_Client_کھڑکی XMUCLIENTWINDOW
#تعریف_کرو Xmu_Print_Default_Error_سنیہا XMUPRINTDEFAULTERRORMESSAGE
#تعریف_کرو XrmGetDatabase XRMGETDATABASE
#تعریف_کرو XrmGetResource XRMGETRESOURCE
#تعریف_کرو XrmPutStringResource XRMPUTSTRINGRESOURCE
#تعریف_کرو Xrm_Quark_منزل_String XRMQUARKTOSTRING
#تعریف_کرو Xrm_String_منزل_Quark XRMSTRINGTOQUARK
#تعریف_کرو Xt_جوڑو_Callback XTADDCALLBACK
#تعریف_کرو Xt_جوڑو_Callbacks XTADDCALLBACKS
#تعریف_کرو Xt_جوڑو_Event_Handler XTADDEVENTHANDLER
#تعریف_کرو Xt_جوڑو_Grab XTADDGRAB
#تعریف_کرو XtAllocateGC XTALLOCATEGC
#تعریف_کرو Xt_App_جوڑو_Actions XTAPPADDACTIONS
#تعریف_کرو Xt_App_جوڑو_داخلہ XTAPPADDINPUT
#تعریف_کرو Xt_App_جوڑو_ویلا_باہرلا XTAPPADDTIMEOUT
#تعریف_کرو XtAppCreateShell XTAPPCREATESHELL
#تعریف_کرو XtAppInitialize XTAPPINITIALIZE
#تعریف_کرو Xt_App_اگلا_Event XTAPPNEXTEVENT
#تعریف_کرو XtAppPeekEvent XTAPPPEEKEVENT
#تعریف_کرو XtAppPending XTAPPPENDING
#تعریف_کرو XtAppProcessEvent XTAPPPROCESSEVENT
#تعریف_کرو Xt_App_سیٹ_Error_Handler XTAPPSETERRORHANDLER
#تعریف_کرو Xt_App_سیٹ_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#تعریف_کرو Xt_App_سیٹ_Warning_Handler XTAPPSETWARNINGHANDLER
#تعریف_کرو Xt_App_سیٹ_Warning_پیغام_Handler XTAPPSETWARNINGMSGHANDLER
#تعریف_کرو XtAppWarning XTAPPWARNING
#تعریف_کرو XtCallActionProc XTCALLACTIONPROC
#تعریف_کرو XtCallCallbackList XTCALLCALLBACKLIST
#تعریف_کرو XtCallCallbacks XTCALLCALLBACKS
#تعریف_کرو XtConfigureWidget XTCONFIGUREWIDGET
#تعریف_کرو XtConvertAndStore XTCONVERTANDSTORE
#تعریف_کرو XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#تعریف_کرو XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#تعریف_کرو XtCreatePopupShell XTCREATEPOPUPSHELL
#تعریف_کرو XtCreateWidget XTCREATEWIDGET
#تعریف_کرو XtDatabase XTDATABASE
#تعریف_کرو XtDestroyWidget XTDESTROYWIDGET
#تعریف_کرو XtDisownSelection XTDISOWNSELECTION
#تعریف_کرو XtDispatchEvent XTDISPATCHEVENT
#تعریف_کرو Xt_Display_دا_Object XTDISPLAYOFOBJECT
#تعریف_کرو XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#تعریف_کرو Xt_Display_منزل_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#تعریف_کرو XtFree XTFREE
#تعریف_کرو XtGetActionKeysym XTGETACTIONKEYSYM
#تعریف_کرو XtGetActionList XTGETACTIONLIST
#تعریف_کرو XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#تعریف_کرو XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#تعریف_کرو XtGetGC XTGETGC
#تعریف_کرو Xt_Get_Multi_Click_ویلا XTGETMULTICLICKTIME
#تعریف_کرو Xt_Get_Selection_قدر XTGETSELECTIONVALUE
#تعریف_کرو Xt_Get_Selection_اقدار XTGETSELECTIONVALUES
#تعریف_کرو XtGetSubresources XTGETSUBRESOURCES
#تعریف_کرو Xt_Get_اقدار XTGETVALUES
#تعریف_کرو XtGrabKeyboard XTGRABKEYBOARD
#تعریف_کرو Xt_Grab_اشارہ_گر XTGRABPOINTER
#تعریف_کرو XtHasCallbacks XTHASCALLBACKS
#تعریف_کرو XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#تعریف_کرو Xt_داخل_کرو_Event_Handler XTINSERTEVENTHANDLER
#تعریف_کرو XtIsManaged XTISMANAGED
#تعریف_کرو XtIsObject XTISOBJECT
#تعریف_کرو XtIsSensitive XTISSENSITIVE
#تعریف_کرو XtIsSubclass XTISSUBCLASS
#تعریف_کرو Xt_آخری_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#تعریف_کرو XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#تعریف_کرو XtMakeResizeRequest XTMAKERESIZEREQUEST
#تعریف_کرو XtMalloc XTMALLOC
#تعریف_کرو Xt_Manage_بچہ XTMANAGECHILD
#تعریف_کرو XtManageChildren XTMANAGECHILDREN
#تعریف_کرو Xt_ملاؤ_Arg_Lists XTMERGEARGLISTS
#تعریف_کرو XtMoveWidget XTMOVEWIDGET
#تعریف_کرو XtName XTNAME
#تعریف_کرو Xt_Name_منزل_Widget XTNAMETOWIDGET
#تعریف_کرو XtOpenDisplay XTOPENDISPLAY
#تعریف_کرو XtOverrideTranslations XTOVERRIDETRANSLATIONS
#تعریف_کرو XtOwnSelection XTOWNSELECTION
#تعریف_کرو Xt_ماپا XTPARENT
#تعریف_کرو Xt_Parse_Translation_جدول XTPARSETRANSLATIONTABLE
#تعریف_کرو XtPopdown XTPOPDOWN
#تعریف_کرو XtPopup XTPOPUP
#تعریف_کرو XtQueryGeometry XTQUERYGEOMETRY
#تعریف_کرو XtRealizeWidget XTREALIZEWIDGET
#تعریف_کرو XtRealloc XTREALLOC
#تعریف_کرو XtReleaseGC XTRELEASEGC
#تعریف_کرو XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#تعریف_کرو XtRemoveCallback XTREMOVECALLBACK
#تعریف_کرو XtRemoveEventHandler XTREMOVEEVENTHANDLER
#تعریف_کرو XtRemoveGrab XTREMOVEGRAB
#تعریف_کرو Xt_Remove_داخلہ XTREMOVEINPUT
#تعریف_کرو Xt_Remove_ویلا_باہرلا XTREMOVETIMEOUT
#تعریف_کرو XtResizeWidget XTRESIZEWIDGET
#تعریف_کرو XtResolvePathname XTRESOLVEPATHNAME
#تعریف_کرو Xt_سیٹ_Keyboard_Focus XTSETKEYBOARDFOCUS
#تعریف_کرو Xt_سیٹ_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#تعریف_کرو Xt_سیٹ_Sensitive XTSETSENSITIVE
#تعریف_کرو Xt_سیٹ_Type_Converter XTSETTYPECONVERTER
#تعریف_کرو Xt_سیٹ_اقدار XTSETVALUES
#تعریف_کرو Xt_Shell_لڑیاں XTSHELLSTRINGS
#تعریف_کرو Xt_لڑیاں XTSTRINGS
#تعریف_کرو XtToolkitInitialize XTTOOLKITINITIALIZE
#تعریف_کرو XtTranslateCoords XTTRANSLATECOORDS
#تعریف_کرو XtTranslateKeycode XTTRANSLATEKEYCODE
#تعریف_کرو XtUngrabKeyboard XTUNGRABKEYBOARD
#تعریف_کرو Xt_Ungrab_اشارہ_گر XTUNGRABPOINTER
#تعریف_کرو Xt_Unmanage_بچہ XTUNMANAGECHILD
#تعریف_کرو XtUnmanageChildren XTUNMANAGECHILDREN
#تعریف_کرو XtUnrealizeWidget XTUNREALIZEWIDGET
#تعریف_کرو XtVaAppCreateShell XTVAAPPCREATESHELL
#تعریف_کرو XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#تعریف_کرو XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#تعریف_کرو XtVaCreateWidget XTVACREATEWIDGET
#تعریف_کرو Xt_Va_Get_اقدار XTVAGETVALUES
#تعریف_کرو Xt_Va_سیٹ_اقدار XTVASETVALUES
#تعریف_کرو XtWarning XTWARNING
#تعریف_کرو Xt_Widget_منزل_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#تعریف_کرو Xt_کھڑکی XTWINDOW
#تعریف_کرو Xt_کھڑکی_دا_Object XTWINDOWOFOBJECT
#تعریف_کرو Xt_کھڑکی_منزل_Widget XTWINDOWTOWIDGET
#تعریف_کرو XwcDrawString XWCDRAWSTRING
#تعریف_کرو XwcFreeStringList XWCFREESTRINGLIST
#تعریف_کرو Xwc_متن_Escapement XWCTEXTESCAPEMENT
#تعریف_کرو Xwc_متن_Extents XWCTEXTEXTENTS
#تعریف_کرو Xwc_متن_List_منزل_متن_Property XWCTEXTLISTTOTEXTPROPERTY
#تعریف_کرو Xwc_متن_Property_منزل_متن_List XWCTEXTPROPERTYTOTEXTLIST
#تعریف_کرو _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#تعریف_کرو _XmClearBorder _XMCLEARBORDER
#تعریف_کرو _XmConfigureObject _XMCONFIGUREOBJECT
#تعریف_کرو Xm_Destroy_ماپا_Callback _XMDESTROYPARENTCALLBACK
#تعریف_کرو _XmDrawArrow _XMDRAWARROW
#تعریف_کرو _XmDrawShadows _XMDRAWSHADOWS
#تعریف_کرو _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#تعریف_کرو _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#تعریف_کرو _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#تعریف_کرو _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#تعریف_کرو _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#تعریف_کرو _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#تعریف_کرو Xm_Get_Widget_Ext_انکڑے _XMGETWIDGETEXTDATA
#تعریف_کرو _XmGrabKeyboard _XMGRABKEYBOARD
#تعریف_کرو Xm_Grab_اشارہ_گر _XMGRABPOINTER
#تعریف_کرو _XmInheritClass _XMINHERITCLASS
#تعریف_کرو Xm_داخلہ_وچ_Gadget _XMINPUTINGADGET
#تعریف_کرو _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#تعریف_کرو Xm_Menu_نکالو_تھلے _XMMENUPOPDOWN
#تعریف_کرو _XmMoveObject _XMMOVEOBJECT
#تعریف_کرو _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#تعریف_کرو _XmOSBuildFileList _XMOSBUILDFILELIST
#تعریف_کرو _XmOSFileCompare _XMOSFILECOMPARE
#تعریف_کرو Xm_OSFind_نمونہ_Part _XMOSFINDPATTERNPART
#تعریف_کرو _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#تعریف_کرو _XmPostPopupMenu _XMPOSTPOPUPMENU
#تعریف_کرو _XmPrimitiveEnter _XMPRIMITIVEENTER
#تعریف_کرو _XmPrimitiveLeave _XMPRIMITIVELEAVE
#تعریف_کرو _XmRedisplayGadgets _XMREDISPLAYGADGETS
#تعریف_کرو _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#تعریف_کرو _XmStringDraw _XMSTRINGDRAW
#تعریف_کرو Xm_String_Get_متن_Concat _XMSTRINGGETTEXTCONCAT
#تعریف_کرو Xm_لڑیاں _XMSTRINGS
#تعریف_کرو Xm_منزل_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#تعریف_کرو Xm_منزل_Vertical_Pixels _XMTOVERTICALPIXELS
#تعریف_کرو Xm_چوٹی_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#تعریف_کرو _Xm_fastPtr _XM_FASTPTR
#تعریف_کرو _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#تعریف_کرو _XtInherit _XTINHERIT
#تعریف_کرو _XtInheritTranslations _XTINHERITTRANSLATIONS
#تعریف_کرو applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#تعریف_کرو مرکب_Widget_Class COMPOSITEWIDGETCLASS
#تعریف_کرو overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#تعریف_کرو shellWidgetClass SHELLWIDGETCLASS
#تعریف_کرو چوٹی_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#تعریف_کرو چوٹی_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#تعریف_کرو transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#تعریف_کرو vendorShellClassRec VENDORSHELLCLASSREC
#تعریف_کرو vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#تعریف_کرو wmShellWidgetClass WMSHELLWIDGETCLASS
#تعریف_کرو xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#تعریف_کرو xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#تعریف_کرو xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#تعریف_کرو xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#تعریف_کرو xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#تعریف_کرو xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#تعریف_کرو xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#تعریف_کرو xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#تعریف_کرو xmFormWidgetClass XMFORMWIDGETCLASS
#تعریف_کرو xm_چوکھٹا_Widget_Class XMFRAMEWIDGETCLASS
#تعریف_کرو xmGadgetClass XMGADGETCLASS
#تعریف_کرو xmLabelGadgetClass XMLABELGADGETCLASS
#تعریف_کرو xmLabelWidgetClass XMLABELWIDGETCLASS
#تعریف_کرو xmListWidgetClass XMLISTWIDGETCLASS
#تعریف_کرو xm_Main_کھڑکی_Widget_Class XMMAINWINDOWWIDGETCLASS
#تعریف_کرو xmManagerClassRec XMMANAGERCLASSREC
#تعریف_کرو xmManagerWidgetClass XMMANAGERWIDGETCLASS
#تعریف_کرو xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#تعریف_کرو xm_سنیہا_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#تعریف_کرو xmPrimitiveClassRec XMPRIMITIVECLASSREC
#تعریف_کرو xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#تعریف_کرو xm_ڈالو_Button_Class_Rec XMPUSHBUTTONCLASSREC
#تعریف_کرو xm_ڈالو_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#تعریف_کرو xm_ڈالو_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#تعریف_کرو xm_صف_کالم_Widget_Class XMROWCOLUMNWIDGETCLASS
#تعریف_کرو xmSashWidgetClass XMSASHWIDGETCLASS
#تعریف_کرو xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#تعریف_کرو xm_Scrolled_کھڑکی_Class_Rec XMSCROLLEDWINDOWCLASSREC
#تعریف_کرو xm_Scrolled_کھڑکی_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#تعریف_کرو xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#تعریف_کرو xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#تعریف_کرو xm_متن_میدان_Widget_Class XMTEXTFIELDWIDGETCLASS
#تعریف_کرو xm_متن_Widget_Class XMTEXTWIDGETCLASS
#تعریف_کرو xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#تعریف_کرو xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#جے_بنا_تعریف VMS_X11R6
// R5 or earlier
#تعریف_کرو Xt_رجسٹر_کھڑکی _XTREGISTERWINDOW
#تعریف_کرو Xt_Unregister_کھڑکی _XTUNREGISTERWINDOW
#وإلا
// R6 or later
#تعریف_کرو Xt_رجسٹر_Drawable XTREGISTERDRAWABLE
#تعریف_کرو XtUnregisterDrawable XTUNREGISTERDRAWABLE
#جے_انت
