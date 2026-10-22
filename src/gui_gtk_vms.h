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

#تعریف_کړه _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#تعریف_کړه XAllocClassHint XALLOCCLASSHINT
#تعریف_کړه XAllocColor XALLOCCOLOR
#تعریف_کړه XAllocColorCells XALLOCCOLORCELLS
#تعریف_کړه XAlloc_اندازه_Hints XALLOCSIZEHINTS
#تعریف_کړه XAllocWMHints XALLOCWMHINTS
#تعریف_کړه XAutoRepeatOff XAUTOREPEATOFF
#تعریف_کړه XAutoRepeatOn XAUTOREPEATON
#تعریف_کړه XBase_Font_Name_List_د_Font_سېټ XBASEFONTNAMELISTOFFONTSET
#تعریف_کړه XBell XBELL
#تعریف_کړه XBitmapPad XBITMAPPAD
#تعریف_کړه XChange_فعال_اشاره_ګر_Grab XCHANGEACTIVEPOINTERGRAB
#تعریف_کړه XChangeGC XCHANGEGC
#تعریف_کړه XChangeProperty XCHANGEPROPERTY
#تعریف_کړه XChange_کړکۍ_ځانګړتیاوې XCHANGEWINDOWATTRIBUTES
#تعریف_کړه XCheckIfEvent XCHECKIFEVENT
#تعریف_کړه XCheck_ماسک_Event XCHECKMASKEVENT
#تعریف_کړه XCheckTypedEvent XCHECKTYPEDEVENT
#تعریف_کړه XCheck_Typed_کړکۍ_Event XCHECKTYPEDWINDOWEVENT
#تعریف_کړه XCheck_کړکۍ_Event XCHECKWINDOWEVENT
#تعریف_کړه XClearArea XCLEARAREA
#تعریف_کړه XClear_کړکۍ XCLEARWINDOW
#تعریف_کړه XClipBox XCLIPBOX
#تعریف_کړه XCloseDisplay XCLOSEDISPLAY
#تعریف_کړه XCloseIM XCLOSEIM
#تعریف_کړه XConfigure_کړکۍ XCONFIGUREWINDOW
#تعریف_کړه XConvertSelection XCONVERTSELECTION
#تعریف_کړه XCopyArea XCOPYAREA
#تعریف_کړه XCopyGC XCOPYGC
#تعریف_کړه XCopyPlane XCOPYPLANE
#تعریف_کړه XCreate_Bitmap_From_معلومات XCREATEBITMAPFROMDATA
#تعریف_کړه XCreateColormap XCREATECOLORMAP
#تعریف_کړه XCreateFontCursor XCREATEFONTCURSOR
#تعریف_کړه XCreate_Font_سېټ XCREATEFONTSET
#تعریف_کړه XCreateGC XCREATEGC
#تعریف_کړه XCreateIC XCREATEIC
#تعریف_کړه XCreateImage XCREATEIMAGE
#تعریف_کړه XCreatePixmap XCREATEPIXMAP
#تعریف_کړه XCreatePixmapCursor XCREATEPIXMAPCURSOR
#تعریف_کړه XCreate_Pixmap_From_Bitmap_معلومات XCREATEPIXMAPFROMBITMAPDATA
#تعریف_کړه XCreateRegion XCREATEREGION
#تعریف_کړه XCreate_Simple_کړکۍ XCREATESIMPLEWINDOW
#تعریف_کړه XCreate_کړکۍ XCREATEWINDOW
#تعریف_کړه XDefault_ریښه_کړکۍ XDEFAULTROOTWINDOW
#تعریف_کړه XDefault_Screen_د_Display XDEFAULTSCREENOFDISPLAY
#تعریف_کړه XDefineCursor XDEFINECURSOR
#تعریف_کړه XDeleteProperty XDELETEPROPERTY
#تعریف_کړه XDestroyIC XDESTROYIC
#تعریف_کړه XDestroyRegion XDESTROYREGION
#تعریف_کړه XDestroy_کړکۍ XDESTROYWINDOW
#تعریف_کړه XDisplayName XDISPLAYNAME
#تعریف_کړه XDisplay_د_Screen XDISPLAYOFSCREEN
#تعریف_کړه XDisplayString XDISPLAYSTRING
#تعریف_کړه XDrawArc XDRAWARC
#تعریف_کړه XDrawImageString XDRAWIMAGESTRING
#تعریف_کړه XDrawImageString16 XDRAWIMAGESTRING16
#تعریف_کړه XDraw_کرښه XDRAWLINE
#تعریف_کړه XDraw_کرښې XDRAWLINES
#تعریف_کړه XDraw_ټکی XDRAWPOINT
#تعریف_کړه XDrawPoints XDRAWPOINTS
#تعریف_کړه XDrawRectangle XDRAWRECTANGLE
#تعریف_کړه XDrawSegments XDRAWSEGMENTS
#تعریف_کړه XDrawString XDRAWSTRING
#تعریف_کړه XDrawString16 XDRAWSTRING16
#تعریف_کړه XEmptyRegion XEMPTYREGION
#تعریف_کړه XEqualRegion XEQUALREGION
#تعریف_کړه XEventsQueued XEVENTSQUEUED
#تعریف_کړه XExtents_د_Font_سېټ XEXTENTSOFFONTSET
#تعریف_کړه XFetch_بفر XFETCHBUFFER
#تعریف_کړه XFillArc XFILLARC
#تعریف_کړه XFillPolygon XFILLPOLYGON
#تعریف_کړه XFillRectangle XFILLRECTANGLE
#تعریف_کړه XFillRectangles XFILLRECTANGLES
#تعریف_کړه XFilterEvent XFILTEREVENT
#تعریف_کړه XFlush XFLUSH
#تعریف_کړه XFonts_د_Font_سېټ XFONTSOFFONTSET
#تعریف_کړه XFree XFREE
#تعریف_کړه XFreeColormap XFREECOLORMAP
#تعریف_کړه XFreeColors XFREECOLORS
#تعریف_کړه XFreeCursor XFREECURSOR
#تعریف_کړه XFreeFont XFREEFONT
#تعریف_کړه XFreeFontInfo XFREEFONTINFO
#تعریف_کړه XFree_Font_نومونه XFREEFONTNAMES
#تعریف_کړه XFree_Font_سېټ XFREEFONTSET
#تعریف_کړه XFreeGC XFREEGC
#تعریف_کړه XFreeModifiermap XFREEMODIFIERMAP
#تعریف_کړه XFreePixmap XFREEPIXMAP
#تعریف_کړه XFreeStringList XFREESTRINGLIST
#تعریف_کړه XGetAtomName XGETATOMNAME
#تعریف_کړه XGetDefault XGETDEFAULT
#تعریف_کړه XGet_Error_Database_متن XGETERRORDATABASETEXT
#تعریف_کړه XGet_Error_متن XGETERRORTEXT
#تعریف_کړه XGetFontProperty XGETFONTPROPERTY
#تعریف_کړه XGetGCValues XGETGCVALUES
#تعریف_کړه XGetGeometry XGETGEOMETRY
#تعریف_کړه XGetIconSizes XGETICONSIZES
#تعریف_کړه XGetICValues XGETICVALUES
#تعریف_کړه XGetIMValues XGETIMVALUES
#تعریف_کړه XGetImage XGETIMAGE
#تعریف_کړه XGetKeyboardControl XGETKEYBOARDCONTROL
#تعریف_کړه XGetModifierMapping XGETMODIFIERMAPPING
#تعریف_کړه XGetMotionEvents XGETMOTIONEVENTS
#تعریف_کړه XGet_نورمال_Hints XGETNORMALHINTS
#تعریف_کړه XGetSelectionOwner XGETSELECTIONOWNER
#تعریف_کړه XGetSubImage XGETSUBIMAGE
#تعریف_کړه XGetVisualInfo XGETVISUALINFO
#تعریف_کړه XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#تعریف_کړه XGetWMIconName XGETWMICONNAME
#تعریف_کړه XGetWMProtocols XGETWMPROTOCOLS
#تعریف_کړه XGetWMHints XGETWMHINTS
#تعریف_کړه XGetWMName XGETWMNAME
#تعریف_کړه XGetWMNormalHints XGETWMNORMALHINTS
#تعریف_کړه XGet_کړکۍ_ځانګړتیاوې XGETWINDOWATTRIBUTES
#تعریف_کړه XGet_کړکۍ_Property XGETWINDOWPROPERTY
#تعریف_کړه XGrabKeyboard XGRABKEYBOARD
#تعریف_کړه XGrab_اشاره_ګر XGRABPOINTER
#تعریف_کړه XGrabServer XGRABSERVER
#تعریف_کړه XHeight_د_Screen XHEIGHTOFSCREEN
#تعریف_کړه XIconify_کړکۍ XICONIFYWINDOW
#تعریف_کړه XIfEvent XIFEVENT
#تعریف_کړه XInternAtom XINTERNATOM
#تعریف_کړه XIntersectRegion XINTERSECTREGION
#تعریف_کړه XKeycode_منزل_Keysym XKEYCODETOKEYSYM
#تعریف_کړه XKeysym_منزل_Keycode XKEYSYMTOKEYCODE
#تعریف_کړه XKeysym_منزل_String XKEYSYMTOSTRING
#تعریف_کړه XListFonts XLISTFONTS
#تعریف_کړه XListFontsWithInfo XLISTFONTSWITHINFO
#تعریف_کړه XListPixmapFormats XLISTPIXMAPFORMATS
#تعریف_کړه XListProperties XLISTPROPERTIES
#تعریف_کړه XLoadQueryFont XLOADQUERYFONT
#تعریف_کړه XLookupString XLOOKUPSTRING
#تعریف_کړه XLower_کړکۍ XLOWERWINDOW
#تعریف_کړه XMapRaised XMAPRAISED
#تعریف_کړه XMap_کړکۍ XMAPWINDOW
#تعریف_کړه XMatchVisualInfo XMATCHVISUALINFO
#تعریف_کړه XMove_Resize_کړکۍ XMOVERESIZEWINDOW
#تعریف_کړه XMove_کړکۍ XMOVEWINDOW
#تعریف_کړه XNextEvent XNEXTEVENT
#تعریف_کړه XOffsetRegion XOFFSETREGION
#تعریف_کړه XOpenDisplay XOPENDISPLAY
#تعریف_کړه XOpenIM XOPENIM
#تعریف_کړه XParseColor XPARSECOLOR
#تعریف_کړه XParseGeometry XPARSEGEOMETRY
#تعریف_کړه XPeekEvent XPEEKEVENT
#تعریف_کړه XPending XPENDING
#تعریف_کړه XPoint_دننه_Region XPOINTINREGION
#تعریف_کړه XPolygonRegion XPOLYGONREGION
#تعریف_کړه XPut_شا_Event XPUTBACKEVENT
#تعریف_کړه XPutImage XPUTIMAGE
#تعریف_کړه XRoot_کړکۍ XROOTWINDOW
#تعریف_کړه XQueryColor XQUERYCOLOR
#تعریف_کړه XQueryColors XQUERYCOLORS
#تعریف_کړه XQueryExtension XQUERYEXTENSION
#تعریف_کړه XQuery_اشاره_ګر XQUERYPOINTER
#تعریف_کړه XQuery_ونه XQUERYTREE
#تعریف_کړه XRaise_کړکۍ XRAISEWINDOW
#تعریف_کړه XReconfigureWMWindow XRECONFIGUREWMWINDOW
#تعریف_کړه XRect_دننه_Region XRECTINREGION
#تعریف_کړه XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#تعریف_کړه XReparent_کړکۍ XREPARENTWINDOW
#تعریف_کړه XResize_کړکۍ XRESIZEWINDOW
#تعریف_کړه XRestackWindows XRESTACKWINDOWS
#تعریف_کړه XRoot_کړکۍ_د_Screen XROOTWINDOWOFSCREEN
#تعریف_کړه XScreen_شمېره_د_Screen XSCREENNUMBEROFSCREEN
#تعریف_کړه XSelectAsyncEvent XSELECTASYNCEVENT
#تعریف_کړه XSelect_Async_ننوت XSELECTASYNCINPUT
#تعریف_کړه XSelect_ننوت XSELECTINPUT
#تعریف_کړه XSendEvent XSENDEVENT
#تعریف_کړه XServerVendor XSERVERVENDOR
#تعریف_کړه XSetBackground XSETBACKGROUND
#تعریف_کړه XSetClassHint XSETCLASSHINT
#تعریف_کړه XSet_Clip_ماسک XSETCLIPMASK
#تعریف_کړه XSetClipOrigin XSETCLIPORIGIN
#تعریف_کړه XSetClipRectangles XSETCLIPRECTANGLES
#تعریف_کړه XSet_Close_ښکته_ډېرپېښ XSETCLOSEDOWNMODE
#تعریف_کړه XSetCommand XSETCOMMAND
#تعریف_کړه XSetDashes XSETDASHES
#تعریف_کړه XSetErrorHandler XSETERRORHANDLER
#تعریف_کړه XSetFillStyle XSETFILLSTYLE
#تعریف_کړه XSetFont XSETFONT
#تعریف_کړه XSetForeground XSETFOREGROUND
#تعریف_کړه XSetFunction XSETFUNCTION
#تعریف_کړه XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#تعریف_کړه XSetICFocus XSETICFOCUS
#تعریف_کړه XSetICValues XSETICVALUES
#تعریف_کړه XSetIOErrorHandler XSETIOERRORHANDLER
#تعریف_کړه XSet_ننوت_Focus XSETINPUTFOCUS
#تعریف_کړه XSet_کرښه_ځانګړتیاوې XSETLINEATTRIBUTES
#تعریف_کړه XSetLocaleModifiers XSETLOCALEMODIFIERS
#تعریف_کړه XSet_نورمال_Hints XSETNORMALHINTS
#تعریف_کړه XSetRegion XSETREGION
#تعریف_کړه XSetSelectionOwner XSETSELECTIONOWNER
#تعریف_کړه XSetStipple XSETSTIPPLE
#تعریف_کړه XSet_Subwindow_ډېرپېښ XSETSUBWINDOWMODE
#تعریف_کړه XSetTSOrigin XSETTSORIGIN
#تعریف_کړه XSetTile XSETTILE
#تعریف_کړه XSetTransientForHint XSETTRANSIENTFORHINT
#تعریف_کړه XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#تعریف_کړه XSetWMHints XSETWMHINTS
#تعریف_کړه XSetWMIconName XSETWMICONNAME
#تعریف_کړه XSetWMName XSETWMNAME
#تعریف_کړه XSetWMNormalHints XSETWMNORMALHINTS
#تعریف_کړه XSetWMProperties XSETWMPROPERTIES
#تعریف_کړه XSetWMProtocols XSETWMPROTOCOLS
#تعریف_کړه XSet_کړکۍ_Background XSETWINDOWBACKGROUND
#تعریف_کړه XSet_کړکۍ_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#تعریف_کړه XSet_کړکۍ_Colormap XSETWINDOWCOLORMAP
#تعریف_کړه XShape_Combine_ماسک XSHAPECOMBINEMASK
#تعریف_کړه XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#تعریف_کړه XShapeGetRectangles XSHAPEGETRECTANGLES
#تعریف_کړه XShrinkRegion XSHRINKREGION
#تعریف_کړه XStore_بفر XSTOREBUFFER
#تعریف_کړه XStoreColor XSTORECOLOR
#تعریف_کړه XStoreColors XSTORECOLORS
#تعریف_کړه XStoreName XSTORENAME
#تعریف_کړه XString_منزل_Keysym XSTRINGTOKEYSYM
#تعریف_کړه XSubtractRegion XSUBTRACTREGION
#تعریف_کړه XSupportsLocale XSUPPORTSLOCALE
#تعریف_کړه XSync XSYNC
#تعریف_کړه XSynchronize XSYNCHRONIZE
#تعریف_کړه XTextExtents XTEXTEXTENTS
#تعریف_کړه XTextExtents16 XTEXTEXTENTS16
#تعریف_کړه XText_پلنوالی XTEXTWIDTH
#تعریف_کړه XTextWidth16 XTEXTWIDTH16
#تعریف_کړه XTranslateCoordinates XTRANSLATECOORDINATES
#تعریف_کړه XUndefineCursor XUNDEFINECURSOR
#تعریف_کړه XUngrabKeyboard XUNGRABKEYBOARD
#تعریف_کړه XUngrab_اشاره_ګر XUNGRABPOINTER
#تعریف_کړه XUngrabServer XUNGRABSERVER
#تعریف_کړه XUnionRectWithRegion XUNIONRECTWITHREGION
#تعریف_کړه XUnionRegion XUNIONREGION
#تعریف_کړه XUnmap_کړکۍ XUNMAPWINDOW
#تعریف_کړه _XUnregisterFilter _XUNREGISTERFILTER
#تعریف_کړه XUnsetICFocus XUNSETICFOCUS
#تعریف_کړه XVaCreateNestedList XVACREATENESTEDLIST
#تعریف_کړه XVisualIDFromVisual XVISUALIDFROMVISUAL
#تعریف_کړه XWarp_اشاره_ګر XWARPPOINTER
#تعریف_کړه XWidth_د_Screen XWIDTHOFSCREEN
#تعریف_کړه XWindowEvent XWINDOWEVENT
#تعریف_کړه XWithdraw_کړکۍ XWITHDRAWWINDOW
#تعریف_کړه XXorRegion XXORREGION
#تعریف_کړه Xm_جمع_کړه_Protocol_Callback XMADDPROTOCOLCALLBACK
#تعریف_کړه Xm_جمع_کړه_Protocols XMADDPROTOCOLS
#تعریف_کړه XmChangeColor XMCHANGECOLOR
#تعریف_کړه XmClipboardCopy XMCLIPBOARDCOPY
#تعریف_کړه Xm_Clipboard_پای_ټکی_Copy XMCLIPBOARDENDCOPY
#تعریف_کړه Xm_Clipboard_Inquire_اوږدتیا XMCLIPBOARDINQUIRELENGTH
#تعریف_کړه Xm_Clipboard_قلف XMCLIPBOARDLOCK
#تعریف_کړه XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#تعریف_کړه Xm_Clipboard_پیل_Copy XMCLIPBOARDSTARTCOPY
#تعریف_کړه XmClipboardUnlock XMCLIPBOARDUNLOCK
#تعریف_کړه XmCreateArrowButton XMCREATEARROWBUTTON
#تعریف_کړه XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#تعریف_کړه XmCreateCascadeButton XMCREATECASCADEBUTTON
#تعریف_کړه XmCreateDialogShell XMCREATEDIALOGSHELL
#تعریف_کړه XmCreateDragIcon XMCREATEDRAGICON
#تعریف_کړه XmCreateDrawingArea XMCREATEDRAWINGAREA
#تعریف_کړه XmCreateDrawnButton XMCREATEDRAWNBUTTON
#تعریف_کړه XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#تعریف_کړه XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#تعریف_کړه XmCreateForm XMCREATEFORM
#تعریف_کړه XmCreateFormDialog XMCREATEFORMDIALOG
#تعریف_کړه Xm_Create_چوکاټ XMCREATEFRAME
#تعریف_کړه XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#تعریف_کړه XmCreateLabelGadget XMCREATELABELGADGET
#تعریف_کړه Xm_Create_Main_کړکۍ XMCREATEMAINWINDOW
#تعریف_کړه XmCreateMenuBar XMCREATEMENUBAR
#تعریف_کړه Xm_Create_پیغام_Box XMCREATEMESSAGEBOX
#تعریف_کړه Xm_Create_پیغام_Dialog XMCREATEMESSAGEDIALOG
#تعریف_کړه XmCreateOptionMenu XMCREATEOPTIONMENU
#تعریف_کړه Xm_Create_Paned_کړکۍ XMCREATEPANEDWINDOW
#تعریف_کړه XmCreatePopupMenu XMCREATEPOPUPMENU
#تعریف_کړه XmCreatePromptDialog XMCREATEPROMPTDIALOG
#تعریف_کړه XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#تعریف_کړه Xm_Create_ټېل_Button XMCREATEPUSHBUTTON
#تعریف_کړه Xm_Create_ټېل_Button_Gadget XMCREATEPUSHBUTTONGADGET
#تعریف_کړه XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#تعریف_کړه XmCreateRadioBox XMCREATERADIOBOX
#تعریف_کړه Xm_Create_لیکه_ستنه XMCREATEROWCOLUMN
#تعریف_کړه XmCreateScale XMCREATESCALE
#تعریف_کړه XmCreateScrollBar XMCREATESCROLLBAR
#تعریف_کړه XmCreateScrolledList XMCREATESCROLLEDLIST
#تعریف_کړه Xm_Create_Scrolled_متن XMCREATESCROLLEDTEXT
#تعریف_کړه Xm_Create_Scrolled_کړکۍ XMCREATESCROLLEDWINDOW
#تعریف_کړه XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#تعریف_کړه XmCreateSeparator XMCREATESEPARATOR
#تعریف_کړه XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#تعریف_کړه XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#تعریف_کړه Xm_Create_متن XMCREATETEXT
#تعریف_کړه Xm_Create_متن_ډګر XMCREATETEXTFIELD
#تعریف_کړه XmCreateToggleButton XMCREATETOGGLEBUTTON
#تعریف_کړه XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#تعریف_کړه Xm_Drag_پیل XMDRAGSTART
#تعریف_کړه Xm_Drop_Site_ثبتوونکی XMDROPSITEREGISTER
#تعریف_کړه XmDropSiteUnregister XMDROPSITEUNREGISTER
#تعریف_کړه XmDropSiteUpdate XMDROPSITEUPDATE
#تعریف_کړه Xm_Drop_Transfer_پیل XMDROPTRANSFERSTART
#تعریف_کړه Xm_File_Selection_Box_Get_ماشوم XMFILESELECTIONBOXGETCHILD
#تعریف_کړه Xm_File_Selection_Do_پلټنه XMFILESELECTIONDOSEARCH
#تعریف_کړه Xm_Font_List_Append_ثبت XMFONTLISTAPPENDENTRY
#تعریف_کړه XmFontListCopy XMFONTLISTCOPY
#تعریف_کړه XmFontListCreate XMFONTLISTCREATE
#تعریف_کړه Xm_Font_List_ثبت_Create XMFONTLISTENTRYCREATE
#تعریف_کړه Xm_Font_List_ثبت_Free XMFONTLISTENTRYFREE
#تعریف_کړه Xm_Font_List_ثبت_Get_Font XMFONTLISTENTRYGETFONT
#تعریف_کړه Xm_Font_List_ثبت_Get_نښان XMFONTLISTENTRYGETTAG
#تعریف_کړه Xm_Font_List_ثبت_بارول XMFONTLISTENTRYLOAD
#تعریف_کړه XmFontListFree XMFONTLISTFREE
#تعریف_کړه XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#تعریف_کړه Xm_Font_List_Get_بل_Font XMFONTLISTGETNEXTFONT
#تعریف_کړه XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#تعریف_کړه Xm_Font_List_بل_ثبت XMFONTLISTNEXTENTRY
#تعریف_کړه XmGetColors XMGETCOLORS
#تعریف_کړه XmGetFocusWidget XMGETFOCUSWIDGET
#تعریف_کړه XmGetMenuCursor XMGETMENUCURSOR
#تعریف_کړه Xm_Get_Pixmap_By_ژوروالی XMGETPIXMAPBYDEPTH
#تعریف_کړه XmGetTearOffControl XMGETTEAROFFCONTROL
#تعریف_کړه XmGetXmDisplay XMGETXMDISPLAY
#تعریف_کړه XmImMbLookupString XMIMMBLOOKUPSTRING
#تعریف_کړه Xm_Im_ثبتوونکی XMIMREGISTER
#تعریف_کړه Xm_Im_سېټ_Focus_ارزښتونه XMIMSETFOCUSVALUES
#تعریف_کړه Xm_Im_سېټ_ارزښتونه XMIMSETVALUES
#تعریف_کړه XmImUnregister XMIMUNREGISTER
#تعریف_کړه XmImUnsetFocus XMIMUNSETFOCUS
#تعریف_کړه XmInternAtom XMINTERNATOM
#تعریف_کړه XmIsMotifWMRunning XMISMOTIFWMRUNNING
#تعریف_کړه Xm_List_جمع_کړه_توکی XMLISTADDITEM
#تعریف_کړه Xm_List_جمع_کړه_توکی_Unselected XMLISTADDITEMUNSELECTED
#تعریف_کړه Xm_List_جمع_کړه_توکي_Unselected XMLISTADDITEMSUNSELECTED
#تعریف_کړه Xm_List_Delete_All_توکي XMLISTDELETEALLITEMS
#تعریف_کړه Xm_List_Delete_توکي_Pos XMLISTDELETEITEMSPOS
#تعریف_کړه XmListDeletePos XMLISTDELETEPOS
#تعریف_کړه Xm_List_Deselect_All_توکي XMLISTDESELECTALLITEMS
#تعریف_کړه XmListDeselectPos XMLISTDESELECTPOS
#تعریف_کړه Xm_List_Get_Kbd_توکی_Pos XMLISTGETKBDITEMPOS
#تعریف_کړه Xm_List_Get_سمون_Pos XMLISTGETMATCHPOS
#تعریف_کړه XmListGetSelectedPos XMLISTGETSELECTEDPOS
#تعریف_کړه XmListPosSelected XMLISTPOSSELECTED
#تعریف_کړه Xm_List_Select_توکی XMLISTSELECTITEM
#تعریف_کړه XmListSelectPos XMLISTSELECTPOS
#تعریف_کړه Xm_List_سېټ_Bottom_Pos XMLISTSETBOTTOMPOS
#تعریف_کړه Xm_List_سېټ_توکی XMLISTSETITEM
#تعریف_کړه Xm_List_سېټ_Kbd_توکی_Pos XMLISTSETKBDITEMPOS
#تعریف_کړه Xm_List_سېټ_Pos XMLISTSETPOS
#تعریف_کړه Xm_Main_کړکۍ_سېټ_Areas XMMAINWINDOWSETAREAS
#تعریف_کړه XmMenuPosition XMMENUPOSITION
#تعریف_کړه Xm_پیغام_Box_Get_ماشوم XMMESSAGEBOXGETCHILD
#تعریف_کړه XmOptionButtonGadget XMOPTIONBUTTONGADGET
#تعریف_کړه XmOptionLabelGadget XMOPTIONLABELGADGET
#تعریف_کړه XmProcessTraversal XMPROCESSTRAVERSAL
#تعریف_کړه XmQmotif XMQMOTIF
#تعریف_کړه XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#تعریف_کړه XmRepTypeGetId XMREPTYPEGETID
#تعریف_کړه Xm_Rep_Type_Get_ریکارډ XMREPTYPEGETRECORD
#تعریف_کړه XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#تعریف_کړه Xm_Rep_Type_ثبتوونکی XMREPTYPEREGISTER
#تعریف_کړه Xm_Rep_Type_Valid_ارزښت XMREPTYPEVALIDVALUE
#تعریف_کړه Xm_Scroll_Bar_سېټ_ارزښتونه XMSCROLLBARSETVALUES
#تعریف_کړه Xm_Scrolled_کړکۍ_سېټ_Areas XMSCROLLEDWINDOWSETAREAS
#تعریف_کړه Xm_Selection_Box_Get_ماشوم XMSELECTIONBOXGETCHILD
#تعریف_کړه XmStringByteCompare XMSTRINGBYTECOMPARE
#تعریف_کړه XmStringCompare XMSTRINGCOMPARE
#تعریف_کړه XmStringConcat XMSTRINGCONCAT
#تعریف_کړه XmStringCopy XMSTRINGCOPY
#تعریف_کړه XmStringCreate XMSTRINGCREATE
#تعریف_کړه XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#تعریف_کړه XmStringCreateLtoR XMSTRINGCREATELTOR
#تعریف_کړه XmStringCreateSimple XMSTRINGCREATESIMPLE
#تعریف_کړه XmStringDraw XMSTRINGDRAW
#تعریف_کړه XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#تعریف_کړه XmStringExtent XMSTRINGEXTENT
#تعریف_کړه XmStringFree XMSTRINGFREE
#تعریف_کړه XmStringFreeContext XMSTRINGFREECONTEXT
#تعریف_کړه XmStringGetLtoR XMSTRINGGETLTOR
#تعریف_کړه Xm_String_Get_بل_Component XMSTRINGGETNEXTCOMPONENT
#تعریف_کړه Xm_String_Get_بل_Segment XMSTRINGGETNEXTSEGMENT
#تعریف_کړه XmStringInitContext XMSTRINGINITCONTEXT
#تعریف_کړه Xm_String_اوږدتیا XMSTRINGLENGTH
#تعریف_کړه XmStringLtoRCreate XMSTRINGLTORCREATE
#تعریف_کړه XmStringNConcat XMSTRINGNCONCAT
#تعریف_کړه XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#تعریف_کړه Xm_String_پلنوالی XMSTRINGWIDTH
#تعریف_کړه Xm_متن_Clear_Selection XMTEXTCLEARSELECTION
#تعریف_کړه Xm_متن_ډګر_Get_Editable XMTEXTFIELDGETEDITABLE
#تعریف_کړه Xm_متن_ډګر_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#تعریف_کړه Xm_متن_ډګر_Get_وروستی_Position XMTEXTFIELDGETLASTPOSITION
#تعریف_کړه Xm_متن_ډګر_Get_Selection XMTEXTFIELDGETSELECTION
#تعریف_کړه Xm_متن_ډګر_Get_String XMTEXTFIELDGETSTRING
#تعریف_کړه Xm_متن_ډګر_ورننباسه XMTEXTFIELDINSERT
#تعریف_کړه Xm_متن_ډګر_Remove XMTEXTFIELDREMOVE
#تعریف_کړه Xm_متن_ډګر_سېټ_Selection XMTEXTFIELDSETSELECTION
#تعریف_کړه Xm_متن_ډګر_سېټ_String XMTEXTFIELDSETSTRING
#تعریف_کړه Xm_متن_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#تعریف_کړه Xm_متن_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#تعریف_کړه Xm_متن_Get_وروستی_Position XMTEXTGETLASTPOSITION
#تعریف_کړه Xm_متن_Get_Max_اوږدتیا XMTEXTGETMAXLENGTH
#تعریف_کړه Xm_متن_Get_Selection XMTEXTGETSELECTION
#تعریف_کړه Xm_متن_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#تعریف_کړه Xm_متن_Get_String XMTEXTGETSTRING
#تعریف_کړه Xm_متن_ورننباسه XMTEXTINSERT
#تعریف_کړه Xm_متن_Remove XMTEXTREMOVE
#تعریف_کړه Xm_متن_Replace XMTEXTREPLACE
#تعریف_کړه Xm_متن_سېټ_Cursor_Position XMTEXTSETCURSORPOSITION
#تعریف_کړه Xm_متن_سېټ_Highlight XMTEXTSETHIGHLIGHT
#تعریف_کړه Xm_متن_سېټ_Insertion_Position XMTEXTSETINSERTIONPOSITION
#تعریف_کړه Xm_متن_سېټ_Selection XMTEXTSETSELECTION
#تعریف_کړه Xm_متن_سېټ_String XMTEXTSETSTRING
#تعریف_کړه XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#تعریف_کړه Xm_Toggle_Button_Gadget_سېټ_State XMTOGGLEBUTTONGADGETSETSTATE
#تعریف_کړه XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#تعریف_کړه Xm_Toggle_Button_سېټ_State XMTOGGLEBUTTONSETSTATE
#تعریف_کړه XmUpdateDisplay XMUPDATEDISPLAY
#تعریف_کړه XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#تعریف_کړه XmbDrawString XMBDRAWSTRING
#تعریف_کړه XmbLookupString XMBLOOKUPSTRING
#تعریف_کړه XmbResetIC XMBRESETIC
#تعریف_کړه Xmb_سېټ_WMProperties XMBSETWMPROPERTIES
#تعریف_کړه Xmb_متن_Escapement XMBTEXTESCAPEMENT
#تعریف_کړه Xmb_متن_Extents XMBTEXTEXTENTS
#تعریف_کړه Xmb_متن_List_منزل_متن_Property XMBTEXTLISTTOTEXTPROPERTY
#تعریف_کړه Xmb_متن_Property_منزل_متن_List XMBTEXTPROPERTYTOTEXTLIST
#تعریف_کړه Xmu_Client_کړکۍ XMUCLIENTWINDOW
#تعریف_کړه Xmu_Print_Default_Error_پیغام XMUPRINTDEFAULTERRORMESSAGE
#تعریف_کړه XrmGetDatabase XRMGETDATABASE
#تعریف_کړه XrmGetResource XRMGETRESOURCE
#تعریف_کړه XrmPutStringResource XRMPUTSTRINGRESOURCE
#تعریف_کړه Xrm_Quark_منزل_String XRMQUARKTOSTRING
#تعریف_کړه Xrm_String_منزل_Quark XRMSTRINGTOQUARK
#تعریف_کړه Xt_جمع_کړه_Callback XTADDCALLBACK
#تعریف_کړه Xt_جمع_کړه_Callbacks XTADDCALLBACKS
#تعریف_کړه Xt_جمع_کړه_Event_Handler XTADDEVENTHANDLER
#تعریف_کړه Xt_جمع_کړه_Grab XTADDGRAB
#تعریف_کړه XtAllocateGC XTALLOCATEGC
#تعریف_کړه Xt_App_جمع_کړه_Actions XTAPPADDACTIONS
#تعریف_کړه Xt_App_جمع_کړه_ننوت XTAPPADDINPUT
#تعریف_کړه Xt_App_جمع_کړه_وخت_بهر XTAPPADDTIMEOUT
#تعریف_کړه XtAppCreateShell XTAPPCREATESHELL
#تعریف_کړه XtAppInitialize XTAPPINITIALIZE
#تعریف_کړه Xt_App_بل_Event XTAPPNEXTEVENT
#تعریف_کړه XtAppPeekEvent XTAPPPEEKEVENT
#تعریف_کړه XtAppPending XTAPPPENDING
#تعریف_کړه XtAppProcessEvent XTAPPPROCESSEVENT
#تعریف_کړه Xt_App_سېټ_Error_Handler XTAPPSETERRORHANDLER
#تعریف_کړه Xt_App_سېټ_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#تعریف_کړه Xt_App_سېټ_Warning_Handler XTAPPSETWARNINGHANDLER
#تعریف_کړه Xt_App_سېټ_Warning_پیغام_Handler XTAPPSETWARNINGMSGHANDLER
#تعریف_کړه XtAppWarning XTAPPWARNING
#تعریف_کړه XtCallActionProc XTCALLACTIONPROC
#تعریف_کړه XtCallCallbackList XTCALLCALLBACKLIST
#تعریف_کړه XtCallCallbacks XTCALLCALLBACKS
#تعریف_کړه XtConfigureWidget XTCONFIGUREWIDGET
#تعریف_کړه XtConvertAndStore XTCONVERTANDSTORE
#تعریف_کړه XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#تعریف_کړه XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#تعریف_کړه XtCreatePopupShell XTCREATEPOPUPSHELL
#تعریف_کړه XtCreateWidget XTCREATEWIDGET
#تعریف_کړه XtDatabase XTDATABASE
#تعریف_کړه XtDestroyWidget XTDESTROYWIDGET
#تعریف_کړه XtDisownSelection XTDISOWNSELECTION
#تعریف_کړه XtDispatchEvent XTDISPATCHEVENT
#تعریف_کړه Xt_Display_د_Object XTDISPLAYOFOBJECT
#تعریف_کړه XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#تعریف_کړه Xt_Display_منزل_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#تعریف_کړه XtFree XTFREE
#تعریف_کړه XtGetActionKeysym XTGETACTIONKEYSYM
#تعریف_کړه XtGetActionList XTGETACTIONLIST
#تعریف_کړه XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#تعریف_کړه XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#تعریف_کړه XtGetGC XTGETGC
#تعریف_کړه Xt_Get_Multi_Click_وخت XTGETMULTICLICKTIME
#تعریف_کړه Xt_Get_Selection_ارزښت XTGETSELECTIONVALUE
#تعریف_کړه Xt_Get_Selection_ارزښتونه XTGETSELECTIONVALUES
#تعریف_کړه XtGetSubresources XTGETSUBRESOURCES
#تعریف_کړه Xt_Get_ارزښتونه XTGETVALUES
#تعریف_کړه XtGrabKeyboard XTGRABKEYBOARD
#تعریف_کړه Xt_Grab_اشاره_ګر XTGRABPOINTER
#تعریف_کړه XtHasCallbacks XTHASCALLBACKS
#تعریف_کړه XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#تعریف_کړه Xt_ورننباسه_Event_Handler XTINSERTEVENTHANDLER
#تعریف_کړه XtIsManaged XTISMANAGED
#تعریف_کړه XtIsObject XTISOBJECT
#تعریف_کړه XtIsSensitive XTISSENSITIVE
#تعریف_کړه XtIsSubclass XTISSUBCLASS
#تعریف_کړه Xt_وروستی_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#تعریف_کړه XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#تعریف_کړه XtMakeResizeRequest XTMAKERESIZEREQUEST
#تعریف_کړه XtMalloc XTMALLOC
#تعریف_کړه Xt_Manage_ماشوم XTMANAGECHILD
#تعریف_کړه XtManageChildren XTMANAGECHILDREN
#تعریف_کړه Xt_یوځای_Arg_Lists XTMERGEARGLISTS
#تعریف_کړه XtMoveWidget XTMOVEWIDGET
#تعریف_کړه XtName XTNAME
#تعریف_کړه Xt_Name_منزل_Widget XTNAMETOWIDGET
#تعریف_کړه XtOpenDisplay XTOPENDISPLAY
#تعریف_کړه XtOverrideTranslations XTOVERRIDETRANSLATIONS
#تعریف_کړه XtOwnSelection XTOWNSELECTION
#تعریف_کړه Xt_پلار XTPARENT
#تعریف_کړه Xt_Parse_Translation_جدول XTPARSETRANSLATIONTABLE
#تعریف_کړه XtPopdown XTPOPDOWN
#تعریف_کړه XtPopup XTPOPUP
#تعریف_کړه XtQueryGeometry XTQUERYGEOMETRY
#تعریف_کړه XtRealizeWidget XTREALIZEWIDGET
#تعریف_کړه XtRealloc XTREALLOC
#تعریف_کړه XtReleaseGC XTRELEASEGC
#تعریف_کړه XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#تعریف_کړه XtRemoveCallback XTREMOVECALLBACK
#تعریف_کړه XtRemoveEventHandler XTREMOVEEVENTHANDLER
#تعریف_کړه XtRemoveGrab XTREMOVEGRAB
#تعریف_کړه Xt_Remove_ننوت XTREMOVEINPUT
#تعریف_کړه Xt_Remove_وخت_بهر XTREMOVETIMEOUT
#تعریف_کړه XtResizeWidget XTRESIZEWIDGET
#تعریف_کړه XtResolvePathname XTRESOLVEPATHNAME
#تعریف_کړه Xt_سېټ_Keyboard_Focus XTSETKEYBOARDFOCUS
#تعریف_کړه Xt_سېټ_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#تعریف_کړه Xt_سېټ_Sensitive XTSETSENSITIVE
#تعریف_کړه Xt_سېټ_Type_Converter XTSETTYPECONVERTER
#تعریف_کړه Xt_سېټ_ارزښتونه XTSETVALUES
#تعریف_کړه Xt_Shell_مزي_ګانې XTSHELLSTRINGS
#تعریف_کړه Xt_مزي_ګانې XTSTRINGS
#تعریف_کړه XtToolkitInitialize XTTOOLKITINITIALIZE
#تعریف_کړه XtTranslateCoords XTTRANSLATECOORDS
#تعریف_کړه XtTranslateKeycode XTTRANSLATEKEYCODE
#تعریف_کړه XtUngrabKeyboard XTUNGRABKEYBOARD
#تعریف_کړه Xt_Ungrab_اشاره_ګر XTUNGRABPOINTER
#تعریف_کړه Xt_Unmanage_ماشوم XTUNMANAGECHILD
#تعریف_کړه XtUnmanageChildren XTUNMANAGECHILDREN
#تعریف_کړه XtUnrealizeWidget XTUNREALIZEWIDGET
#تعریف_کړه XtVaAppCreateShell XTVAAPPCREATESHELL
#تعریف_کړه XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#تعریف_کړه XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#تعریف_کړه XtVaCreateWidget XTVACREATEWIDGET
#تعریف_کړه Xt_Va_Get_ارزښتونه XTVAGETVALUES
#تعریف_کړه Xt_Va_سېټ_ارزښتونه XTVASETVALUES
#تعریف_کړه XtWarning XTWARNING
#تعریف_کړه Xt_Widget_منزل_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#تعریف_کړه Xt_کړکۍ XTWINDOW
#تعریف_کړه Xt_کړکۍ_د_Object XTWINDOWOFOBJECT
#تعریف_کړه Xt_کړکۍ_منزل_Widget XTWINDOWTOWIDGET
#تعریف_کړه XwcDrawString XWCDRAWSTRING
#تعریف_کړه XwcFreeStringList XWCFREESTRINGLIST
#تعریف_کړه Xwc_متن_Escapement XWCTEXTESCAPEMENT
#تعریف_کړه Xwc_متن_Extents XWCTEXTEXTENTS
#تعریف_کړه Xwc_متن_List_منزل_متن_Property XWCTEXTLISTTOTEXTPROPERTY
#تعریف_کړه Xwc_متن_Property_منزل_متن_List XWCTEXTPROPERTYTOTEXTLIST
#تعریف_کړه _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#تعریف_کړه _XmClearBorder _XMCLEARBORDER
#تعریف_کړه _XmConfigureObject _XMCONFIGUREOBJECT
#تعریف_کړه Xm_Destroy_پلار_Callback _XMDESTROYPARENTCALLBACK
#تعریف_کړه _XmDrawArrow _XMDRAWARROW
#تعریف_کړه _XmDrawShadows _XMDRAWSHADOWS
#تعریف_کړه _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#تعریف_کړه _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#تعریف_کړه _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#تعریف_کړه _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#تعریف_کړه _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#تعریف_کړه _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#تعریف_کړه Xm_Get_Widget_Ext_معلومات _XMGETWIDGETEXTDATA
#تعریف_کړه _XmGrabKeyboard _XMGRABKEYBOARD
#تعریف_کړه Xm_Grab_اشاره_ګر _XMGRABPOINTER
#تعریف_کړه _XmInheritClass _XMINHERITCLASS
#تعریف_کړه Xm_ننوت_دننه_Gadget _XMINPUTINGADGET
#تعریف_کړه _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#تعریف_کړه Xm_Menu_راوباسه_ښکته _XMMENUPOPDOWN
#تعریف_کړه _XmMoveObject _XMMOVEOBJECT
#تعریف_کړه _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#تعریف_کړه _XmOSBuildFileList _XMOSBUILDFILELIST
#تعریف_کړه _XmOSFileCompare _XMOSFILECOMPARE
#تعریف_کړه Xm_OSFind_نمونه_Part _XMOSFINDPATTERNPART
#تعریف_کړه _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#تعریف_کړه _XmPostPopupMenu _XMPOSTPOPUPMENU
#تعریف_کړه _XmPrimitiveEnter _XMPRIMITIVEENTER
#تعریف_کړه _XmPrimitiveLeave _XMPRIMITIVELEAVE
#تعریف_کړه _XmRedisplayGadgets _XMREDISPLAYGADGETS
#تعریف_کړه _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#تعریف_کړه _XmStringDraw _XMSTRINGDRAW
#تعریف_کړه Xm_String_Get_متن_Concat _XMSTRINGGETTEXTCONCAT
#تعریف_کړه Xm_مزي_ګانې _XMSTRINGS
#تعریف_کړه Xm_منزل_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#تعریف_کړه Xm_منزل_Vertical_Pixels _XMTOVERTICALPIXELS
#تعریف_کړه Xm_سر_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#تعریف_کړه _Xm_fastPtr _XM_FASTPTR
#تعریف_کړه _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#تعریف_کړه _XtInherit _XTINHERIT
#تعریف_کړه _XtInheritTranslations _XTINHERITTRANSLATIONS
#تعریف_کړه applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#تعریف_کړه مرکب_Widget_Class COMPOSITEWIDGETCLASS
#تعریف_کړه overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#تعریف_کړه shellWidgetClass SHELLWIDGETCLASS
#تعریف_کړه سر_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#تعریف_کړه سر_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#تعریف_کړه transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#تعریف_کړه vendorShellClassRec VENDORSHELLCLASSREC
#تعریف_کړه vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#تعریف_کړه wmShellWidgetClass WMSHELLWIDGETCLASS
#تعریف_کړه xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#تعریف_کړه xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#تعریف_کړه xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#تعریف_کړه xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#تعریف_کړه xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#تعریف_کړه xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#تعریف_کړه xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#تعریف_کړه xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#تعریف_کړه xmFormWidgetClass XMFORMWIDGETCLASS
#تعریف_کړه xm_چوکاټ_Widget_Class XMFRAMEWIDGETCLASS
#تعریف_کړه xmGadgetClass XMGADGETCLASS
#تعریف_کړه xmLabelGadgetClass XMLABELGADGETCLASS
#تعریف_کړه xmLabelWidgetClass XMLABELWIDGETCLASS
#تعریف_کړه xmListWidgetClass XMLISTWIDGETCLASS
#تعریف_کړه xm_Main_کړکۍ_Widget_Class XMMAINWINDOWWIDGETCLASS
#تعریف_کړه xmManagerClassRec XMMANAGERCLASSREC
#تعریف_کړه xmManagerWidgetClass XMMANAGERWIDGETCLASS
#تعریف_کړه xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#تعریف_کړه xm_پیغام_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#تعریف_کړه xmPrimitiveClassRec XMPRIMITIVECLASSREC
#تعریف_کړه xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#تعریف_کړه xm_ټېل_Button_Class_Rec XMPUSHBUTTONCLASSREC
#تعریف_کړه xm_ټېل_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#تعریف_کړه xm_ټېل_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#تعریف_کړه xm_لیکه_ستنه_Widget_Class XMROWCOLUMNWIDGETCLASS
#تعریف_کړه xmSashWidgetClass XMSASHWIDGETCLASS
#تعریف_کړه xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#تعریف_کړه xm_Scrolled_کړکۍ_Class_Rec XMSCROLLEDWINDOWCLASSREC
#تعریف_کړه xm_Scrolled_کړکۍ_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#تعریف_کړه xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#تعریف_کړه xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#تعریف_کړه xm_متن_ډګر_Widget_Class XMTEXTFIELDWIDGETCLASS
#تعریف_کړه xm_متن_Widget_Class XMTEXTWIDGETCLASS
#تعریف_کړه xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#تعریف_کړه xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#که_نه_تعریف_شوی VMS_X11R6
// R5 or earlier
#تعریف_کړه Xt_ثبتوونکی_کړکۍ _XTREGISTERWINDOW
#تعریف_کړه Xt_Unregister_کړکۍ _XTUNREGISTERWINDOW
#وإلا
// R6 or later
#تعریف_کړه Xt_ثبتوونکی_Drawable XTREGISTERDRAWABLE
#تعریف_کړه XtUnregisterDrawable XTUNREGISTERDRAWABLE
#د_که_پای
