// this file contains the actual definitions of
// the IIDs and CLSIDs

// link this file in with the server and any clients


// File created by MIDL compiler version 3.00.44
// at Sat Jan 03 16:34:55 1998
// Compiler settings for if_ole.idl:
// Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
// error checks: none
//@@MIDL_FILE_HEADING(  )
#که_تعریف_شوی __cplusplus
خارجي "C"{
#د_که_پای

#که_تعریف_شوی __MINGW32__
# include <w32api.h>

# إذا __W32API_MAJOR_VERSION == 3 && __W32API_MINOR_VERSION < 10
   // This define is missing from older MingW versions of w32api, even though
   // IID is defined.
#  define __IID_DEFINED__
# endif
#د_که_پای

#که_نه_تعریف_شوی __IID_DEFINED__
# define __IID_DEFINED__

تعريفنوع بنية _IID
{
    بینښه اوږد x;
    بینښه لنډ s1;
    بینښه لنډ s2;
    بینښه حرف  c[8];
} IID;

#د_که_پای

#که_نه_تعریف_شوی CLSID_DEFINED
# define CLSID_DEFINED
تعريفنوع IID CLSID;
#د_که_پای

ثابت IID IID_IVim = {0x0F0BFAE2,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ثابت IID LIBID_Vim = {0x0F0BFAE0,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ثابت CLSID CLSID_Vim = {0x0F0BFAE1,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


#که_تعریف_شوی __cplusplus
}
#د_که_پای

