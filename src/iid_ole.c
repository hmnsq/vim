// this file contains the actual definitions of
// the IIDs and CLSIDs

// link this file in with the server and any clients


// File created by MIDL compiler version 3.00.44
// at Sat Jan 03 16:34:55 1998
// Compiler settings for if_ole.idl:
// Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
// error checks: none
//@@MIDL_FILE_HEADING(  )
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C"{
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# include <w32api.h>

# ಆದರೆ __W32API_MAJOR_VERSION == 3 && __W32API_MINOR_VERSION < 10
   // This define is missing from older MingW versions of w32api, even though
   // IID is defined.
#  define __IID_DEFINED__
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __IID_DEFINED__
# define __IID_DEFINED__

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _IID
{
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ x;
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ s1;
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ s2;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  c[8];
} IID;

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CLSID_DEFINED
# define CLSID_DEFINED
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ IID CLSID;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ IID IID_IVim = {0x0F0BFAE2,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ಸ್ಥಿರಾಂಕ IID LIBID_Vim = {0x0F0BFAE0,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


ಸ್ಥಿರಾಂಕ CLSID CLSID_Vim = {0x0F0BFAE1,0x4C90,0x11d1,{0x82,0xD7,0x00,0x04,0xAC,0x36,0x85,0x19}};


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ

