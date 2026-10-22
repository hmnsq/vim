// this file contains the actual definitions of
// the IIDs and CLSIDs

// link this file in with the server and any clients


// File created by MIDL compiler version 3.00.44
// at Sat Jan 03 16:34:55 1998
// Compiler settings for if_ole.idl:
// Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
// error checks: none
//@@MIDL_FILE_HEADING(  )
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C"{
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# include <w32api.h>

# ਜੇ __W32API_MAJOR_VERSION == 3 && __W32API_MINOR_VERSION < 10
   // This define is missing from older MingW versions of w32api, even though
   // IID is defined.
#  define __IID_DEFINED__
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __IID_DEFINED__
# define __IID_DEFINED__

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _IID
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ x;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ s1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ s2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  c[8];
} IID;

#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CLSID_DEFINED
# define CLSID_DEFINED
ਕਿਸਮਪਰਿਭਾਸ਼ਾ IID CLSID;
#ਜੇ_ਅੰਤ

ਅਚਲ IID IID_IVim = {0x0F0BFAE2,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ਅਚਲ IID LIBID_Vim = {0x0F0BFAE0,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ਅਚਲ CLSID CLSID_Vim = {0x0F0BFAE1,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ

