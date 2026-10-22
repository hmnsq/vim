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

#تعریف_کن _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#تعریف_کن XAllocClassHint XALLOCCLASSHINT
#تعریف_کن XAllocColor XALLOCCOLOR
#تعریف_کن XAllocColorCells XALLOCCOLORCELLS
#تعریف_کن XAlloc_اندازه_Hints XALLOCSIZEHINTS
#تعریف_کن XAllocWMHints XALLOCWMHINTS
#تعریف_کن XAutoRepeatOff XAUTOREPEATOFF
#تعریف_کن XAutoRepeatOn XAUTOREPEATON
#تعریف_کن XBase_Font_Name_List_از_Font_مجموعه XBASEFONTNAMELISTOFFONTSET
#تعریف_کن XBell XBELL
#تعریف_کن XBitmapPad XBITMAPPAD
#تعریف_کن XChange_فعال_اشاره_گر_Grab XCHANGEACTIVEPOINTERGRAB
#تعریف_کن XChangeGC XCHANGEGC
#تعریف_کن XChangeProperty XCHANGEPROPERTY
#تعریف_کن XChange_پنجره_صفات XCHANGEWINDOWATTRIBUTES
#تعریف_کن XCheckIfEvent XCHECKIFEVENT
#تعریف_کن XCheck_ماسک_Event XCHECKMASKEVENT
#تعریف_کن XCheckTypedEvent XCHECKTYPEDEVENT
#تعریف_کن XCheck_Typed_پنجره_Event XCHECKTYPEDWINDOWEVENT
#تعریف_کن XCheck_پنجره_Event XCHECKWINDOWEVENT
#تعریف_کن XClearArea XCLEARAREA
#تعریف_کن XClear_پنجره XCLEARWINDOW
#تعریف_کن XClipBox XCLIPBOX
#تعریف_کن XCloseDisplay XCLOSEDISPLAY
#تعریف_کن XCloseIM XCLOSEIM
#تعریف_کن XConfigure_پنجره XCONFIGUREWINDOW
#تعریف_کن XConvertSelection XCONVERTSELECTION
#تعریف_کن XCopyArea XCOPYAREA
#تعریف_کن XCopyGC XCOPYGC
#تعریف_کن XCopyPlane XCOPYPLANE
#تعریف_کن XCreate_Bitmap_From_داده XCREATEBITMAPFROMDATA
#تعریف_کن XCreateColormap XCREATECOLORMAP
#تعریف_کن XCreateFontCursor XCREATEFONTCURSOR
#تعریف_کن XCreate_Font_مجموعه XCREATEFONTSET
#تعریف_کن XCreateGC XCREATEGC
#تعریف_کن XCreateIC XCREATEIC
#تعریف_کن XCreateImage XCREATEIMAGE
#تعریف_کن XCreatePixmap XCREATEPIXMAP
#تعریف_کن XCreatePixmapCursor XCREATEPIXMAPCURSOR
#تعریف_کن XCreate_Pixmap_From_Bitmap_داده XCREATEPIXMAPFROMBITMAPDATA
#تعریف_کن XCreateRegion XCREATEREGION
#تعریف_کن XCreate_Simple_پنجره XCREATESIMPLEWINDOW
#تعریف_کن XCreate_پنجره XCREATEWINDOW
#تعریف_کن XDefault_ریشه_پنجره XDEFAULTROOTWINDOW
#تعریف_کن XDefault_Screen_از_Display XDEFAULTSCREENOFDISPLAY
#تعریف_کن XDefineCursor XDEFINECURSOR
#تعریف_کن XDeleteProperty XDELETEPROPERTY
#تعریف_کن XDestroyIC XDESTROYIC
#تعریف_کن XDestroyRegion XDESTROYREGION
#تعریف_کن XDestroy_پنجره XDESTROYWINDOW
#تعریف_کن XDisplayName XDISPLAYNAME
#تعریف_کن XDisplay_از_Screen XDISPLAYOFSCREEN
#تعریف_کن XDisplayString XDISPLAYSTRING
#تعریف_کن XDrawArc XDRAWARC
#تعریف_کن XDrawImageString XDRAWIMAGESTRING
#تعریف_کن XDrawImageString16 XDRAWIMAGESTRING16
#تعریف_کن XDraw_سطر XDRAWLINE
#تعریف_کن XDraw_سطرها XDRAWLINES
#تعریف_کن XDraw_نقطه XDRAWPOINT
#تعریف_کن XDrawPoints XDRAWPOINTS
#تعریف_کن XDrawRectangle XDRAWRECTANGLE
#تعریف_کن XDrawSegments XDRAWSEGMENTS
#تعریف_کن XDrawString XDRAWSTRING
#تعریف_کن XDrawString16 XDRAWSTRING16
#تعریف_کن XEmptyRegion XEMPTYREGION
#تعریف_کن XEqualRegion XEQUALREGION
#تعریف_کن XEventsQueued XEVENTSQUEUED
#تعریف_کن XExtents_از_Font_مجموعه XEXTENTSOFFONTSET
#تعریف_کن XFetch_میانگیر XFETCHBUFFER
#تعریف_کن XFillArc XFILLARC
#تعریف_کن XFillPolygon XFILLPOLYGON
#تعریف_کن XFillRectangle XFILLRECTANGLE
#تعریف_کن XFillRectangles XFILLRECTANGLES
#تعریف_کن XFilterEvent XFILTEREVENT
#تعریف_کن XFlush XFLUSH
#تعریف_کن XFonts_از_Font_مجموعه XFONTSOFFONTSET
#تعریف_کن XFree XFREE
#تعریف_کن XFreeColormap XFREECOLORMAP
#تعریف_کن XFreeColors XFREECOLORS
#تعریف_کن XFreeCursor XFREECURSOR
#تعریف_کن XFreeFont XFREEFONT
#تعریف_کن XFreeFontInfo XFREEFONTINFO
#تعریف_کن XFree_Font_نامها XFREEFONTNAMES
#تعریف_کن XFree_Font_مجموعه XFREEFONTSET
#تعریف_کن XFreeGC XFREEGC
#تعریف_کن XFreeModifiermap XFREEMODIFIERMAP
#تعریف_کن XFreePixmap XFREEPIXMAP
#تعریف_کن XFreeStringList XFREESTRINGLIST
#تعریف_کن XGetAtomName XGETATOMNAME
#تعریف_کن XGetDefault XGETDEFAULT
#تعریف_کن XGet_Error_Database_متن XGETERRORDATABASETEXT
#تعریف_کن XGet_Error_متن XGETERRORTEXT
#تعریف_کن XGetFontProperty XGETFONTPROPERTY
#تعریف_کن XGetGCValues XGETGCVALUES
#تعریف_کن XGetGeometry XGETGEOMETRY
#تعریف_کن XGetIconSizes XGETICONSIZES
#تعریف_کن XGetICValues XGETICVALUES
#تعریف_کن XGetIMValues XGETIMVALUES
#تعریف_کن XGetImage XGETIMAGE
#تعریف_کن XGetKeyboardControl XGETKEYBOARDCONTROL
#تعریف_کن XGetModifierMapping XGETMODIFIERMAPPING
#تعریف_کن XGetMotionEvents XGETMOTIONEVENTS
#تعریف_کن XGet_نرمال_Hints XGETNORMALHINTS
#تعریف_کن XGetSelectionOwner XGETSELECTIONOWNER
#تعریف_کن XGetSubImage XGETSUBIMAGE
#تعریف_کن XGetVisualInfo XGETVISUALINFO
#تعریف_کن XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#تعریف_کن XGetWMIconName XGETWMICONNAME
#تعریف_کن XGetWMProtocols XGETWMPROTOCOLS
#تعریف_کن XGetWMHints XGETWMHINTS
#تعریف_کن XGetWMName XGETWMNAME
#تعریف_کن XGetWMNormalHints XGETWMNORMALHINTS
#تعریف_کن XGet_پنجره_صفات XGETWINDOWATTRIBUTES
#تعریف_کن XGet_پنجره_Property XGETWINDOWPROPERTY
#تعریف_کن XGrabKeyboard XGRABKEYBOARD
#تعریف_کن XGrab_اشاره_گر XGRABPOINTER
#تعریف_کن XGrabServer XGRABSERVER
#تعریف_کن XHeight_از_Screen XHEIGHTOFSCREEN
#تعریف_کن XIconify_پنجره XICONIFYWINDOW
#تعریف_کن XIfEvent XIFEVENT
#تعریف_کن XInternAtom XINTERNATOM
#تعریف_کن XIntersectRegion XINTERSECTREGION
#تعریف_کن XKeycode_مقصد_Keysym XKEYCODETOKEYSYM
#تعریف_کن XKeysym_مقصد_Keycode XKEYSYMTOKEYCODE
#تعریف_کن XKeysym_مقصد_String XKEYSYMTOSTRING
#تعریف_کن XListFonts XLISTFONTS
#تعریف_کن XListFontsWithInfo XLISTFONTSWITHINFO
#تعریف_کن XListPixmapFormats XLISTPIXMAPFORMATS
#تعریف_کن XListProperties XLISTPROPERTIES
#تعریف_کن XLoadQueryFont XLOADQUERYFONT
#تعریف_کن XLookupString XLOOKUPSTRING
#تعریف_کن XLower_پنجره XLOWERWINDOW
#تعریف_کن XMapRaised XMAPRAISED
#تعریف_کن XMap_پنجره XMAPWINDOW
#تعریف_کن XMatchVisualInfo XMATCHVISUALINFO
#تعریف_کن XMove_Resize_پنجره XMOVERESIZEWINDOW
#تعریف_کن XMove_پنجره XMOVEWINDOW
#تعریف_کن XNextEvent XNEXTEVENT
#تعریف_کن XOffsetRegion XOFFSETREGION
#تعریف_کن XOpenDisplay XOPENDISPLAY
#تعریف_کن XOpenIM XOPENIM
#تعریف_کن XParseColor XPARSECOLOR
#تعریف_کن XParseGeometry XPARSEGEOMETRY
#تعریف_کن XPeekEvent XPEEKEVENT
#تعریف_کن XPending XPENDING
#تعریف_کن XPoint_درون_Region XPOINTINREGION
#تعریف_کن XPolygonRegion XPOLYGONREGION
#تعریف_کن XPut_پشت_Event XPUTBACKEVENT
#تعریف_کن XPutImage XPUTIMAGE
#تعریف_کن XRoot_پنجره XROOTWINDOW
#تعریف_کن XQueryColor XQUERYCOLOR
#تعریف_کن XQueryColors XQUERYCOLORS
#تعریف_کن XQueryExtension XQUERYEXTENSION
#تعریف_کن XQuery_اشاره_گر XQUERYPOINTER
#تعریف_کن XQuery_درخت XQUERYTREE
#تعریف_کن XRaise_پنجره XRAISEWINDOW
#تعریف_کن XReconfigureWMWindow XRECONFIGUREWMWINDOW
#تعریف_کن XRect_درون_Region XRECTINREGION
#تعریف_کن XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#تعریف_کن XReparent_پنجره XREPARENTWINDOW
#تعریف_کن XResize_پنجره XRESIZEWINDOW
#تعریف_کن XRestackWindows XRESTACKWINDOWS
#تعریف_کن XRoot_پنجره_از_Screen XROOTWINDOWOFSCREEN
#تعریف_کن XScreen_عدد_از_Screen XSCREENNUMBEROFSCREEN
#تعریف_کن XSelectAsyncEvent XSELECTASYNCEVENT
#تعریف_کن XSelect_Async_درون_داد XSELECTASYNCINPUT
#تعریف_کن XSelect_درون_داد XSELECTINPUT
#تعریف_کن XSendEvent XSENDEVENT
#تعریف_کن XServerVendor XSERVERVENDOR
#تعریف_کن XSetBackground XSETBACKGROUND
#تعریف_کن XSetClassHint XSETCLASSHINT
#تعریف_کن XSet_Clip_ماسک XSETCLIPMASK
#تعریف_کن XSetClipOrigin XSETCLIPORIGIN
#تعریف_کن XSetClipRectangles XSETCLIPRECTANGLES
#تعریف_کن XSet_Close_پایین_نما XSETCLOSEDOWNMODE
#تعریف_کن XSetCommand XSETCOMMAND
#تعریف_کن XSetDashes XSETDASHES
#تعریف_کن XSetErrorHandler XSETERRORHANDLER
#تعریف_کن XSetFillStyle XSETFILLSTYLE
#تعریف_کن XSetFont XSETFONT
#تعریف_کن XSetForeground XSETFOREGROUND
#تعریف_کن XSetFunction XSETFUNCTION
#تعریف_کن XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#تعریف_کن XSetICFocus XSETICFOCUS
#تعریف_کن XSetICValues XSETICVALUES
#تعریف_کن XSetIOErrorHandler XSETIOERRORHANDLER
#تعریف_کن XSet_درون_داد_Focus XSETINPUTFOCUS
#تعریف_کن XSet_سطر_صفات XSETLINEATTRIBUTES
#تعریف_کن XSetLocaleModifiers XSETLOCALEMODIFIERS
#تعریف_کن XSet_نرمال_Hints XSETNORMALHINTS
#تعریف_کن XSetRegion XSETREGION
#تعریف_کن XSetSelectionOwner XSETSELECTIONOWNER
#تعریف_کن XSetStipple XSETSTIPPLE
#تعریف_کن XSet_Subwindow_نما XSETSUBWINDOWMODE
#تعریف_کن XSetTSOrigin XSETTSORIGIN
#تعریف_کن XSetTile XSETTILE
#تعریف_کن XSetTransientForHint XSETTRANSIENTFORHINT
#تعریف_کن XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#تعریف_کن XSetWMHints XSETWMHINTS
#تعریف_کن XSetWMIconName XSETWMICONNAME
#تعریف_کن XSetWMName XSETWMNAME
#تعریف_کن XSetWMNormalHints XSETWMNORMALHINTS
#تعریف_کن XSetWMProperties XSETWMPROPERTIES
#تعریف_کن XSetWMProtocols XSETWMPROTOCOLS
#تعریف_کن XSet_پنجره_Background XSETWINDOWBACKGROUND
#تعریف_کن XSet_پنجره_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#تعریف_کن XSet_پنجره_Colormap XSETWINDOWCOLORMAP
#تعریف_کن XShape_Combine_ماسک XSHAPECOMBINEMASK
#تعریف_کن XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#تعریف_کن XShapeGetRectangles XSHAPEGETRECTANGLES
#تعریف_کن XShrinkRegion XSHRINKREGION
#تعریف_کن XStore_میانگیر XSTOREBUFFER
#تعریف_کن XStoreColor XSTORECOLOR
#تعریف_کن XStoreColors XSTORECOLORS
#تعریف_کن XStoreName XSTORENAME
#تعریف_کن XString_مقصد_Keysym XSTRINGTOKEYSYM
#تعریف_کن XSubtractRegion XSUBTRACTREGION
#تعریف_کن XSupportsLocale XSUPPORTSLOCALE
#تعریف_کن XSync XSYNC
#تعریف_کن XSynchronize XSYNCHRONIZE
#تعریف_کن XTextExtents XTEXTEXTENTS
#تعریف_کن XTextExtents16 XTEXTEXTENTS16
#تعریف_کن XText_عرض XTEXTWIDTH
#تعریف_کن XTextWidth16 XTEXTWIDTH16
#تعریف_کن XTranslateCoordinates XTRANSLATECOORDINATES
#تعریف_کن XUndefineCursor XUNDEFINECURSOR
#تعریف_کن XUngrabKeyboard XUNGRABKEYBOARD
#تعریف_کن XUngrab_اشاره_گر XUNGRABPOINTER
#تعریف_کن XUngrabServer XUNGRABSERVER
#تعریف_کن XUnionRectWithRegion XUNIONRECTWITHREGION
#تعریف_کن XUnionRegion XUNIONREGION
#تعریف_کن XUnmap_پنجره XUNMAPWINDOW
#تعریف_کن _XUnregisterFilter _XUNREGISTERFILTER
#تعریف_کن XUnsetICFocus XUNSETICFOCUS
#تعریف_کن XVaCreateNestedList XVACREATENESTEDLIST
#تعریف_کن XVisualIDFromVisual XVISUALIDFROMVISUAL
#تعریف_کن XWarp_اشاره_گر XWARPPOINTER
#تعریف_کن XWidth_از_Screen XWIDTHOFSCREEN
#تعریف_کن XWindowEvent XWINDOWEVENT
#تعریف_کن XWithdraw_پنجره XWITHDRAWWINDOW
#تعریف_کن XXorRegion XXORREGION
#تعریف_کن Xm_جمع_کن_Protocol_Callback XMADDPROTOCOLCALLBACK
#تعریف_کن Xm_جمع_کن_Protocols XMADDPROTOCOLS
#تعریف_کن XmChangeColor XMCHANGECOLOR
#تعریف_کن XmClipboardCopy XMCLIPBOARDCOPY
#تعریف_کن Xm_Clipboard_انتها_Copy XMCLIPBOARDENDCOPY
#تعریف_کن Xm_Clipboard_Inquire_درازی XMCLIPBOARDINQUIRELENGTH
#تعریف_کن Xm_Clipboard_قفل XMCLIPBOARDLOCK
#تعریف_کن XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#تعریف_کن Xm_Clipboard_شروع_Copy XMCLIPBOARDSTARTCOPY
#تعریف_کن XmClipboardUnlock XMCLIPBOARDUNLOCK
#تعریف_کن XmCreateArrowButton XMCREATEARROWBUTTON
#تعریف_کن XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#تعریف_کن XmCreateCascadeButton XMCREATECASCADEBUTTON
#تعریف_کن XmCreateDialogShell XMCREATEDIALOGSHELL
#تعریف_کن XmCreateDragIcon XMCREATEDRAGICON
#تعریف_کن XmCreateDrawingArea XMCREATEDRAWINGAREA
#تعریف_کن XmCreateDrawnButton XMCREATEDRAWNBUTTON
#تعریف_کن XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#تعریف_کن XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#تعریف_کن XmCreateForm XMCREATEFORM
#تعریف_کن XmCreateFormDialog XMCREATEFORMDIALOG
#تعریف_کن Xm_Create_قاب XMCREATEFRAME
#تعریف_کن XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#تعریف_کن XmCreateLabelGadget XMCREATELABELGADGET
#تعریف_کن Xm_Create_Main_پنجره XMCREATEMAINWINDOW
#تعریف_کن XmCreateMenuBar XMCREATEMENUBAR
#تعریف_کن Xm_Create_پیام_Box XMCREATEMESSAGEBOX
#تعریف_کن Xm_Create_پیام_Dialog XMCREATEMESSAGEDIALOG
#تعریف_کن XmCreateOptionMenu XMCREATEOPTIONMENU
#تعریف_کن Xm_Create_Paned_پنجره XMCREATEPANEDWINDOW
#تعریف_کن XmCreatePopupMenu XMCREATEPOPUPMENU
#تعریف_کن XmCreatePromptDialog XMCREATEPROMPTDIALOG
#تعریف_کن XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#تعریف_کن Xm_Create_افزودن_Button XMCREATEPUSHBUTTON
#تعریف_کن Xm_Create_افزودن_Button_Gadget XMCREATEPUSHBUTTONGADGET
#تعریف_کن XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#تعریف_کن XmCreateRadioBox XMCREATERADIOBOX
#تعریف_کن Xm_Create_ردیف_ستونی XMCREATEROWCOLUMN
#تعریف_کن XmCreateScale XMCREATESCALE
#تعریف_کن XmCreateScrollBar XMCREATESCROLLBAR
#تعریف_کن XmCreateScrolledList XMCREATESCROLLEDLIST
#تعریف_کن Xm_Create_Scrolled_متن XMCREATESCROLLEDTEXT
#تعریف_کن Xm_Create_Scrolled_پنجره XMCREATESCROLLEDWINDOW
#تعریف_کن XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#تعریف_کن XmCreateSeparator XMCREATESEPARATOR
#تعریف_کن XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#تعریف_کن XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#تعریف_کن Xm_Create_متن XMCREATETEXT
#تعریف_کن Xm_Create_متن_فیلد XMCREATETEXTFIELD
#تعریف_کن XmCreateToggleButton XMCREATETOGGLEBUTTON
#تعریف_کن XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#تعریف_کن Xm_Drag_شروع XMDRAGSTART
#تعریف_کن Xm_Drop_Site_ثبات XMDROPSITEREGISTER
#تعریف_کن XmDropSiteUnregister XMDROPSITEUNREGISTER
#تعریف_کن XmDropSiteUpdate XMDROPSITEUPDATE
#تعریف_کن Xm_Drop_Transfer_شروع XMDROPTRANSFERSTART
#تعریف_کن Xm_File_Selection_Box_Get_فرزند XMFILESELECTIONBOXGETCHILD
#تعریف_کن Xm_File_Selection_Do_جستجو XMFILESELECTIONDOSEARCH
#تعریف_کن Xm_Font_List_Append_مدخل XMFONTLISTAPPENDENTRY
#تعریف_کن XmFontListCopy XMFONTLISTCOPY
#تعریف_کن XmFontListCreate XMFONTLISTCREATE
#تعریف_کن Xm_Font_List_مدخل_Create XMFONTLISTENTRYCREATE
#تعریف_کن Xm_Font_List_مدخل_Free XMFONTLISTENTRYFREE
#تعریف_کن Xm_Font_List_مدخل_Get_Font XMFONTLISTENTRYGETFONT
#تعریف_کن Xm_Font_List_مدخل_Get_برچسب XMFONTLISTENTRYGETTAG
#تعریف_کن Xm_Font_List_مدخل_بارگیری XMFONTLISTENTRYLOAD
#تعریف_کن XmFontListFree XMFONTLISTFREE
#تعریف_کن XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#تعریف_کن Xm_Font_List_Get_بعدی_Font XMFONTLISTGETNEXTFONT
#تعریف_کن XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#تعریف_کن Xm_Font_List_بعدی_مدخل XMFONTLISTNEXTENTRY
#تعریف_کن XmGetColors XMGETCOLORS
#تعریف_کن XmGetFocusWidget XMGETFOCUSWIDGET
#تعریف_کن XmGetMenuCursor XMGETMENUCURSOR
#تعریف_کن Xm_Get_Pixmap_By_عمق XMGETPIXMAPBYDEPTH
#تعریف_کن XmGetTearOffControl XMGETTEAROFFCONTROL
#تعریف_کن XmGetXmDisplay XMGETXMDISPLAY
#تعریف_کن XmImMbLookupString XMIMMBLOOKUPSTRING
#تعریف_کن Xm_Im_ثبات XMIMREGISTER
#تعریف_کن Xm_Im_مجموعه_Focus_مقادیر XMIMSETFOCUSVALUES
#تعریف_کن Xm_Im_مجموعه_مقادیر XMIMSETVALUES
#تعریف_کن XmImUnregister XMIMUNREGISTER
#تعریف_کن XmImUnsetFocus XMIMUNSETFOCUS
#تعریف_کن XmInternAtom XMINTERNATOM
#تعریف_کن XmIsMotifWMRunning XMISMOTIFWMRUNNING
#تعریف_کن Xm_List_جمع_کن_قلم XMLISTADDITEM
#تعریف_کن Xm_List_جمع_کن_قلم_Unselected XMLISTADDITEMUNSELECTED
#تعریف_کن Xm_List_جمع_کن_اقلام_Unselected XMLISTADDITEMSUNSELECTED
#تعریف_کن Xm_List_Delete_All_اقلام XMLISTDELETEALLITEMS
#تعریف_کن Xm_List_Delete_اقلام_Pos XMLISTDELETEITEMSPOS
#تعریف_کن XmListDeletePos XMLISTDELETEPOS
#تعریف_کن Xm_List_Deselect_All_اقلام XMLISTDESELECTALLITEMS
#تعریف_کن XmListDeselectPos XMLISTDESELECTPOS
#تعریف_کن Xm_List_Get_Kbd_قلم_Pos XMLISTGETKBDITEMPOS
#تعریف_کن Xm_List_Get_تطابق_Pos XMLISTGETMATCHPOS
#تعریف_کن XmListGetSelectedPos XMLISTGETSELECTEDPOS
#تعریف_کن XmListPosSelected XMLISTPOSSELECTED
#تعریف_کن Xm_List_Select_قلم XMLISTSELECTITEM
#تعریف_کن XmListSelectPos XMLISTSELECTPOS
#تعریف_کن Xm_List_مجموعه_Bottom_Pos XMLISTSETBOTTOMPOS
#تعریف_کن Xm_List_مجموعه_قلم XMLISTSETITEM
#تعریف_کن Xm_List_مجموعه_Kbd_قلم_Pos XMLISTSETKBDITEMPOS
#تعریف_کن Xm_List_مجموعه_Pos XMLISTSETPOS
#تعریف_کن Xm_Main_پنجره_مجموعه_Areas XMMAINWINDOWSETAREAS
#تعریف_کن XmMenuPosition XMMENUPOSITION
#تعریف_کن Xm_پیام_Box_Get_فرزند XMMESSAGEBOXGETCHILD
#تعریف_کن XmOptionButtonGadget XMOPTIONBUTTONGADGET
#تعریف_کن XmOptionLabelGadget XMOPTIONLABELGADGET
#تعریف_کن XmProcessTraversal XMPROCESSTRAVERSAL
#تعریف_کن XmQmotif XMQMOTIF
#تعریف_کن XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#تعریف_کن XmRepTypeGetId XMREPTYPEGETID
#تعریف_کن Xm_Rep_Type_Get_رکورد XMREPTYPEGETRECORD
#تعریف_کن XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#تعریف_کن Xm_Rep_Type_ثبات XMREPTYPEREGISTER
#تعریف_کن Xm_Rep_Type_Valid_مقدار XMREPTYPEVALIDVALUE
#تعریف_کن Xm_Scroll_Bar_مجموعه_مقادیر XMSCROLLBARSETVALUES
#تعریف_کن Xm_Scrolled_پنجره_مجموعه_Areas XMSCROLLEDWINDOWSETAREAS
#تعریف_کن Xm_Selection_Box_Get_فرزند XMSELECTIONBOXGETCHILD
#تعریف_کن XmStringByteCompare XMSTRINGBYTECOMPARE
#تعریف_کن XmStringCompare XMSTRINGCOMPARE
#تعریف_کن XmStringConcat XMSTRINGCONCAT
#تعریف_کن XmStringCopy XMSTRINGCOPY
#تعریف_کن XmStringCreate XMSTRINGCREATE
#تعریف_کن XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#تعریف_کن XmStringCreateLtoR XMSTRINGCREATELTOR
#تعریف_کن XmStringCreateSimple XMSTRINGCREATESIMPLE
#تعریف_کن XmStringDraw XMSTRINGDRAW
#تعریف_کن XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#تعریف_کن XmStringExtent XMSTRINGEXTENT
#تعریف_کن XmStringFree XMSTRINGFREE
#تعریف_کن XmStringFreeContext XMSTRINGFREECONTEXT
#تعریف_کن XmStringGetLtoR XMSTRINGGETLTOR
#تعریف_کن Xm_String_Get_بعدی_Component XMSTRINGGETNEXTCOMPONENT
#تعریف_کن Xm_String_Get_بعدی_Segment XMSTRINGGETNEXTSEGMENT
#تعریف_کن XmStringInitContext XMSTRINGINITCONTEXT
#تعریف_کن Xm_String_درازی XMSTRINGLENGTH
#تعریف_کن XmStringLtoRCreate XMSTRINGLTORCREATE
#تعریف_کن XmStringNConcat XMSTRINGNCONCAT
#تعریف_کن XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#تعریف_کن Xm_String_عرض XMSTRINGWIDTH
#تعریف_کن Xm_متن_Clear_Selection XMTEXTCLEARSELECTION
#تعریف_کن Xm_متن_فیلد_Get_Editable XMTEXTFIELDGETEDITABLE
#تعریف_کن Xm_متن_فیلد_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#تعریف_کن Xm_متن_فیلد_Get_آخرین_Position XMTEXTFIELDGETLASTPOSITION
#تعریف_کن Xm_متن_فیلد_Get_Selection XMTEXTFIELDGETSELECTION
#تعریف_کن Xm_متن_فیلد_Get_String XMTEXTFIELDGETSTRING
#تعریف_کن Xm_متن_فیلد_درج XMTEXTFIELDINSERT
#تعریف_کن Xm_متن_فیلد_Remove XMTEXTFIELDREMOVE
#تعریف_کن Xm_متن_فیلد_مجموعه_Selection XMTEXTFIELDSETSELECTION
#تعریف_کن Xm_متن_فیلد_مجموعه_String XMTEXTFIELDSETSTRING
#تعریف_کن Xm_متن_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#تعریف_کن Xm_متن_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#تعریف_کن Xm_متن_Get_آخرین_Position XMTEXTGETLASTPOSITION
#تعریف_کن Xm_متن_Get_Max_درازی XMTEXTGETMAXLENGTH
#تعریف_کن Xm_متن_Get_Selection XMTEXTGETSELECTION
#تعریف_کن Xm_متن_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#تعریف_کن Xm_متن_Get_String XMTEXTGETSTRING
#تعریف_کن Xm_متن_درج XMTEXTINSERT
#تعریف_کن Xm_متن_Remove XMTEXTREMOVE
#تعریف_کن Xm_متن_Replace XMTEXTREPLACE
#تعریف_کن Xm_متن_مجموعه_Cursor_Position XMTEXTSETCURSORPOSITION
#تعریف_کن Xm_متن_مجموعه_Highlight XMTEXTSETHIGHLIGHT
#تعریف_کن Xm_متن_مجموعه_Insertion_Position XMTEXTSETINSERTIONPOSITION
#تعریف_کن Xm_متن_مجموعه_Selection XMTEXTSETSELECTION
#تعریف_کن Xm_متن_مجموعه_String XMTEXTSETSTRING
#تعریف_کن XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#تعریف_کن Xm_Toggle_Button_Gadget_مجموعه_State XMTOGGLEBUTTONGADGETSETSTATE
#تعریف_کن XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#تعریف_کن Xm_Toggle_Button_مجموعه_State XMTOGGLEBUTTONSETSTATE
#تعریف_کن XmUpdateDisplay XMUPDATEDISPLAY
#تعریف_کن XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#تعریف_کن XmbDrawString XMBDRAWSTRING
#تعریف_کن XmbLookupString XMBLOOKUPSTRING
#تعریف_کن XmbResetIC XMBRESETIC
#تعریف_کن Xmb_مجموعه_WMProperties XMBSETWMPROPERTIES
#تعریف_کن Xmb_متن_Escapement XMBTEXTESCAPEMENT
#تعریف_کن Xmb_متن_Extents XMBTEXTEXTENTS
#تعریف_کن Xmb_متن_List_مقصد_متن_Property XMBTEXTLISTTOTEXTPROPERTY
#تعریف_کن Xmb_متن_Property_مقصد_متن_List XMBTEXTPROPERTYTOTEXTLIST
#تعریف_کن Xmu_Client_پنجره XMUCLIENTWINDOW
#تعریف_کن Xmu_Print_Default_Error_پیام XMUPRINTDEFAULTERRORMESSAGE
#تعریف_کن XrmGetDatabase XRMGETDATABASE
#تعریف_کن XrmGetResource XRMGETRESOURCE
#تعریف_کن XrmPutStringResource XRMPUTSTRINGRESOURCE
#تعریف_کن Xrm_Quark_مقصد_String XRMQUARKTOSTRING
#تعریف_کن Xrm_String_مقصد_Quark XRMSTRINGTOQUARK
#تعریف_کن Xt_جمع_کن_Callback XTADDCALLBACK
#تعریف_کن Xt_جمع_کن_Callbacks XTADDCALLBACKS
#تعریف_کن Xt_جمع_کن_Event_Handler XTADDEVENTHANDLER
#تعریف_کن Xt_جمع_کن_Grab XTADDGRAB
#تعریف_کن XtAllocateGC XTALLOCATEGC
#تعریف_کن Xt_App_جمع_کن_Actions XTAPPADDACTIONS
#تعریف_کن Xt_App_جمع_کن_درون_داد XTAPPADDINPUT
#تعریف_کن Xt_App_جمع_کن_زمان_بیرون XTAPPADDTIMEOUT
#تعریف_کن XtAppCreateShell XTAPPCREATESHELL
#تعریف_کن XtAppInitialize XTAPPINITIALIZE
#تعریف_کن Xt_App_بعدی_Event XTAPPNEXTEVENT
#تعریف_کن XtAppPeekEvent XTAPPPEEKEVENT
#تعریف_کن XtAppPending XTAPPPENDING
#تعریف_کن XtAppProcessEvent XTAPPPROCESSEVENT
#تعریف_کن Xt_App_مجموعه_Error_Handler XTAPPSETERRORHANDLER
#تعریف_کن Xt_App_مجموعه_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#تعریف_کن Xt_App_مجموعه_Warning_Handler XTAPPSETWARNINGHANDLER
#تعریف_کن Xt_App_مجموعه_Warning_پیام_Handler XTAPPSETWARNINGMSGHANDLER
#تعریف_کن XtAppWarning XTAPPWARNING
#تعریف_کن XtCallActionProc XTCALLACTIONPROC
#تعریف_کن XtCallCallbackList XTCALLCALLBACKLIST
#تعریف_کن XtCallCallbacks XTCALLCALLBACKS
#تعریف_کن XtConfigureWidget XTCONFIGUREWIDGET
#تعریف_کن XtConvertAndStore XTCONVERTANDSTORE
#تعریف_کن XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#تعریف_کن XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#تعریف_کن XtCreatePopupShell XTCREATEPOPUPSHELL
#تعریف_کن XtCreateWidget XTCREATEWIDGET
#تعریف_کن XtDatabase XTDATABASE
#تعریف_کن XtDestroyWidget XTDESTROYWIDGET
#تعریف_کن XtDisownSelection XTDISOWNSELECTION
#تعریف_کن XtDispatchEvent XTDISPATCHEVENT
#تعریف_کن Xt_Display_از_Object XTDISPLAYOFOBJECT
#تعریف_کن XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#تعریف_کن Xt_Display_مقصد_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#تعریف_کن XtFree XTFREE
#تعریف_کن XtGetActionKeysym XTGETACTIONKEYSYM
#تعریف_کن XtGetActionList XTGETACTIONLIST
#تعریف_کن XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#تعریف_کن XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#تعریف_کن XtGetGC XTGETGC
#تعریف_کن Xt_Get_Multi_Click_زمان XTGETMULTICLICKTIME
#تعریف_کن Xt_Get_Selection_مقدار XTGETSELECTIONVALUE
#تعریف_کن Xt_Get_Selection_مقادیر XTGETSELECTIONVALUES
#تعریف_کن XtGetSubresources XTGETSUBRESOURCES
#تعریف_کن Xt_Get_مقادیر XTGETVALUES
#تعریف_کن XtGrabKeyboard XTGRABKEYBOARD
#تعریف_کن Xt_Grab_اشاره_گر XTGRABPOINTER
#تعریف_کن XtHasCallbacks XTHASCALLBACKS
#تعریف_کن XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#تعریف_کن Xt_درج_Event_Handler XTINSERTEVENTHANDLER
#تعریف_کن XtIsManaged XTISMANAGED
#تعریف_کن XtIsObject XTISOBJECT
#تعریف_کن XtIsSensitive XTISSENSITIVE
#تعریف_کن XtIsSubclass XTISSUBCLASS
#تعریف_کن Xt_آخرین_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#تعریف_کن XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#تعریف_کن XtMakeResizeRequest XTMAKERESIZEREQUEST
#تعریف_کن XtMalloc XTMALLOC
#تعریف_کن Xt_Manage_فرزند XTMANAGECHILD
#تعریف_کن XtManageChildren XTMANAGECHILDREN
#تعریف_کن Xt_ادغام_Arg_Lists XTMERGEARGLISTS
#تعریف_کن XtMoveWidget XTMOVEWIDGET
#تعریف_کن XtName XTNAME
#تعریف_کن Xt_Name_مقصد_Widget XTNAMETOWIDGET
#تعریف_کن XtOpenDisplay XTOPENDISPLAY
#تعریف_کن XtOverrideTranslations XTOVERRIDETRANSLATIONS
#تعریف_کن XtOwnSelection XTOWNSELECTION
#تعریف_کن Xt_والد XTPARENT
#تعریف_کن Xt_Parse_Translation_جدول XTPARSETRANSLATIONTABLE
#تعریف_کن XtPopdown XTPOPDOWN
#تعریف_کن XtPopup XTPOPUP
#تعریف_کن XtQueryGeometry XTQUERYGEOMETRY
#تعریف_کن XtRealizeWidget XTREALIZEWIDGET
#تعریف_کن XtRealloc XTREALLOC
#تعریف_کن XtReleaseGC XTRELEASEGC
#تعریف_کن XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#تعریف_کن XtRemoveCallback XTREMOVECALLBACK
#تعریف_کن XtRemoveEventHandler XTREMOVEEVENTHANDLER
#تعریف_کن XtRemoveGrab XTREMOVEGRAB
#تعریف_کن Xt_Remove_درون_داد XTREMOVEINPUT
#تعریف_کن Xt_Remove_زمان_بیرون XTREMOVETIMEOUT
#تعریف_کن XtResizeWidget XTRESIZEWIDGET
#تعریف_کن XtResolvePathname XTRESOLVEPATHNAME
#تعریف_کن Xt_مجموعه_Keyboard_Focus XTSETKEYBOARDFOCUS
#تعریف_کن Xt_مجموعه_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#تعریف_کن Xt_مجموعه_Sensitive XTSETSENSITIVE
#تعریف_کن Xt_مجموعه_Type_Converter XTSETTYPECONVERTER
#تعریف_کن Xt_مجموعه_مقادیر XTSETVALUES
#تعریف_کن Xt_Shell_رشتهها XTSHELLSTRINGS
#تعریف_کن Xt_رشتهها XTSTRINGS
#تعریف_کن XtToolkitInitialize XTTOOLKITINITIALIZE
#تعریف_کن XtTranslateCoords XTTRANSLATECOORDS
#تعریف_کن XtTranslateKeycode XTTRANSLATEKEYCODE
#تعریف_کن XtUngrabKeyboard XTUNGRABKEYBOARD
#تعریف_کن Xt_Ungrab_اشاره_گر XTUNGRABPOINTER
#تعریف_کن Xt_Unmanage_فرزند XTUNMANAGECHILD
#تعریف_کن XtUnmanageChildren XTUNMANAGECHILDREN
#تعریف_کن XtUnrealizeWidget XTUNREALIZEWIDGET
#تعریف_کن XtVaAppCreateShell XTVAAPPCREATESHELL
#تعریف_کن XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#تعریف_کن XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#تعریف_کن XtVaCreateWidget XTVACREATEWIDGET
#تعریف_کن Xt_Va_Get_مقادیر XTVAGETVALUES
#تعریف_کن Xt_Va_مجموعه_مقادیر XTVASETVALUES
#تعریف_کن XtWarning XTWARNING
#تعریف_کن Xt_Widget_مقصد_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#تعریف_کن Xt_پنجره XTWINDOW
#تعریف_کن Xt_پنجره_از_Object XTWINDOWOFOBJECT
#تعریف_کن Xt_پنجره_مقصد_Widget XTWINDOWTOWIDGET
#تعریف_کن XwcDrawString XWCDRAWSTRING
#تعریف_کن XwcFreeStringList XWCFREESTRINGLIST
#تعریف_کن Xwc_متن_Escapement XWCTEXTESCAPEMENT
#تعریف_کن Xwc_متن_Extents XWCTEXTEXTENTS
#تعریف_کن Xwc_متن_List_مقصد_متن_Property XWCTEXTLISTTOTEXTPROPERTY
#تعریف_کن Xwc_متن_Property_مقصد_متن_List XWCTEXTPROPERTYTOTEXTLIST
#تعریف_کن _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#تعریف_کن _XmClearBorder _XMCLEARBORDER
#تعریف_کن _XmConfigureObject _XMCONFIGUREOBJECT
#تعریف_کن Xm_Destroy_والد_Callback _XMDESTROYPARENTCALLBACK
#تعریف_کن _XmDrawArrow _XMDRAWARROW
#تعریف_کن _XmDrawShadows _XMDRAWSHADOWS
#تعریف_کن _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#تعریف_کن _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#تعریف_کن _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#تعریف_کن _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#تعریف_کن _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#تعریف_کن _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#تعریف_کن Xm_Get_Widget_Ext_داده _XMGETWIDGETEXTDATA
#تعریف_کن _XmGrabKeyboard _XMGRABKEYBOARD
#تعریف_کن Xm_Grab_اشاره_گر _XMGRABPOINTER
#تعریف_کن _XmInheritClass _XMINHERITCLASS
#تعریف_کن Xm_درون_داد_درون_Gadget _XMINPUTINGADGET
#تعریف_کن _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#تعریف_کن Xm_Menu_برداشت_پایین _XMMENUPOPDOWN
#تعریف_کن _XmMoveObject _XMMOVEOBJECT
#تعریف_کن _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#تعریف_کن _XmOSBuildFileList _XMOSBUILDFILELIST
#تعریف_کن _XmOSFileCompare _XMOSFILECOMPARE
#تعریف_کن Xm_OSFind_الگو_Part _XMOSFINDPATTERNPART
#تعریف_کن _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#تعریف_کن _XmPostPopupMenu _XMPOSTPOPUPMENU
#تعریف_کن _XmPrimitiveEnter _XMPRIMITIVEENTER
#تعریف_کن _XmPrimitiveLeave _XMPRIMITIVELEAVE
#تعریف_کن _XmRedisplayGadgets _XMREDISPLAYGADGETS
#تعریف_کن _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#تعریف_کن _XmStringDraw _XMSTRINGDRAW
#تعریف_کن Xm_String_Get_متن_Concat _XMSTRINGGETTEXTCONCAT
#تعریف_کن Xm_رشتهها _XMSTRINGS
#تعریف_کن Xm_مقصد_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#تعریف_کن Xm_مقصد_Vertical_Pixels _XMTOVERTICALPIXELS
#تعریف_کن Xm_بالاترین_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#تعریف_کن _Xm_fastPtr _XM_FASTPTR
#تعریف_کن _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#تعریف_کن _XtInherit _XTINHERIT
#تعریف_کن _XtInheritTranslations _XTINHERITTRANSLATIONS
#تعریف_کن applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#تعریف_کن مرکب_Widget_Class COMPOSITEWIDGETCLASS
#تعریف_کن overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#تعریف_کن shellWidgetClass SHELLWIDGETCLASS
#تعریف_کن بالاترین_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#تعریف_کن بالاترین_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#تعریف_کن transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#تعریف_کن vendorShellClassRec VENDORSHELLCLASSREC
#تعریف_کن vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#تعریف_کن wmShellWidgetClass WMSHELLWIDGETCLASS
#تعریف_کن xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#تعریف_کن xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#تعریف_کن xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#تعریف_کن xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#تعریف_کن xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#تعریف_کن xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#تعریف_کن xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#تعریف_کن xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#تعریف_کن xmFormWidgetClass XMFORMWIDGETCLASS
#تعریف_کن xm_قاب_Widget_Class XMFRAMEWIDGETCLASS
#تعریف_کن xmGadgetClass XMGADGETCLASS
#تعریف_کن xmLabelGadgetClass XMLABELGADGETCLASS
#تعریف_کن xmLabelWidgetClass XMLABELWIDGETCLASS
#تعریف_کن xmListWidgetClass XMLISTWIDGETCLASS
#تعریف_کن xm_Main_پنجره_Widget_Class XMMAINWINDOWWIDGETCLASS
#تعریف_کن xmManagerClassRec XMMANAGERCLASSREC
#تعریف_کن xmManagerWidgetClass XMMANAGERWIDGETCLASS
#تعریف_کن xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#تعریف_کن xm_پیام_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#تعریف_کن xmPrimitiveClassRec XMPRIMITIVECLASSREC
#تعریف_کن xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#تعریف_کن xm_افزودن_Button_Class_Rec XMPUSHBUTTONCLASSREC
#تعریف_کن xm_افزودن_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#تعریف_کن xm_افزودن_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#تعریف_کن xm_ردیف_ستونی_Widget_Class XMROWCOLUMNWIDGETCLASS
#تعریف_کن xmSashWidgetClass XMSASHWIDGETCLASS
#تعریف_کن xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#تعریف_کن xm_Scrolled_پنجره_Class_Rec XMSCROLLEDWINDOWCLASSREC
#تعریف_کن xm_Scrolled_پنجره_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#تعریف_کن xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#تعریف_کن xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#تعریف_کن xm_متن_فیلد_Widget_Class XMTEXTFIELDWIDGETCLASS
#تعریف_کن xm_متن_Widget_Class XMTEXTWIDGETCLASS
#تعریف_کن xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#تعریف_کن xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#اگر_تعریف_نشده VMS_X11R6
// R5 or earlier
#تعریف_کن Xt_ثبات_پنجره _XTREGISTERWINDOW
#تعریف_کن Xt_Unregister_پنجره _XTUNREGISTERWINDOW
#وإلا
// R6 or later
#تعریف_کن Xt_ثبات_Drawable XTREGISTERDRAWABLE
#تعریف_کن XtUnregisterDrawable XTUNREGISTERDRAWABLE
#پایان_اگر
