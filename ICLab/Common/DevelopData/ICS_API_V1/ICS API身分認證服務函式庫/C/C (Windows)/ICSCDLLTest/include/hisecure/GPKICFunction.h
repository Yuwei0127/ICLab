#ifndef __GDCFUNCTION_DLL__
#define __GDCFUNCTION_DLL__

//////////////////////////////////////////////////////////////////
//			            G&D IC卡相關函式						//
//////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
unsigned long __stdcall GetCertificateFromGPKICard( int	iCertID, 
													unsigned char	*ppucCertificate,
													int				*piCertificateLength,
													char			*sReaderName);

extern "C" __declspec(dllexport)
unsigned long __stdcall GPKICardInitialize(char			*sReaderName);

extern "C" __declspec(dllexport)
unsigned long __stdcall GPKICardClose(void);

extern "C" __declspec(dllexport)
unsigned long __stdcall GPKICardVerifyUserPIN(char *pinCode);

extern "C" __declspec(dllexport)
unsigned long __stdcall GPKICardReadCardID(char *smartCardID);

extern "C" __declspec(dllexport)
unsigned long __stdcall GPKICardChangePIN(char *pCurrentPIN, char *pNewPIN);

#endif
