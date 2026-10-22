// this file contains the actual definitions of
// the IIDs and CLSIDs

// link this file in with the server and any clients


// File created by MIDL compiler version 3.00.44
// at Sat Jan 03 16:34:55 1998
// Compiler settings for if_ole.idl:
// Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
// error checks: none
//@@MIDL_FILE_HEADING(  )
#جے_تعریف_شدہ __cplusplus
خارجي "C"{
#جے_انت

#جے_تعریف_شدہ __MINGW32__
# include <w32api.h>

# إذا __W32API_MAJOR_VERSION == 3 && __W32API_MINOR_VERSION < 10
   // This define is missing from older MingW versions of w32api, even though
   // IID is defined.
#  define __IID_DEFINED__
# endif
#جے_انت

#جے_بنا_تعریف __IID_DEFINED__
# define __IID_DEFINED__

تعريفنوع بنية _IID
{
    بنانشانی لما x;
    بنانشانی چھوٹا s1;
    بنانشانی چھوٹا s2;
    بنانشانی حرف  c[8];
} IID;

#جے_انت

#جے_بنا_تعریف CLSID_DEFINED
# define CLSID_DEFINED
تعريفنوع IID CLSID;
#جے_انت

ثابت IID IID_IVim = {0x0F0BFAE2,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ثابت IID LIBID_Vim = {0x0F0BFAE0,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ثابت CLSID CLSID_Vim = {0x0F0BFAE1,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


#جے_تعریف_شدہ __cplusplus
}
#جے_انت

