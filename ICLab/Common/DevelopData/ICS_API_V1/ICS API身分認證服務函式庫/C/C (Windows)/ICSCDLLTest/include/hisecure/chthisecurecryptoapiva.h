#ifndef __CHTHISECURECRYPTOAPI_DLL__
#define __CHTHISECURECRYPTOAPI_DLL__

#include "chthisecurestructva.h"


//////////////////////////////////////////////////////////////////
//						啟用與關閉相關函式						//
//////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
unsigned long __stdcall InitModule(char*			pszModuleName,
								   void*			pInitArgs,
								   unsigned long*	pulModuleHandle);

extern "C" __declspec(dllexport)
unsigned long __stdcall CloseModule(unsigned long	ulModuleHandle);

extern "C" __declspec(dllexport)
unsigned long __stdcall InitSession(unsigned long	ulModuleHandle,
								    int				iFlags,	 
									char*			pszUserPin,
									int 			iUserPinLength,
									unsigned long*	pulSessionHandle);

extern "C" __declspec(dllexport)
unsigned long __stdcall CloseSession(unsigned long	ulModuleHandle,
									 unsigned long	ulSessionHandle);

//////////////////////////////////////////////////////////////////
//   		   				物件相關函式						//
//////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
unsigned long __stdcall GenerateGCAv3PrivateKeyStruct(unsigned char*		pGCAv3PrivateKey,	int ipGCAv3PrivateKeyLength,
													  char*					sPassword,			int iPasswordLength,
													  PrivateKeyInputData&	PvkStruct);

extern "C" __declspec(dllexport)
unsigned long __stdcall GetKeyObjectHandle(unsigned long	ulModuleHandle,
							  			   unsigned long	ulSessionHandle,
										   int				iKeyType,
										   unsigned char*	pKeyID,				int		iKeyIDLength,
										   void*			pvPara,				int		iParaLength,
										   unsigned long*	pulKeyObjectHandle);

extern "C" __declspec(dllexport)
unsigned long __stdcall DeleteKeyObject(unsigned long	ulModuleHandle,
							  		  	unsigned long	ulSessionHandle,
										unsigned long	pulKeyObjectHandle);


//////////////////////////////////////////////////////////////////
//   		   			    雜湊函式							//
//////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
unsigned long __stdcall HashFunction(unsigned long	ulModuleHandle,
							  		 unsigned long	ulSessionHandle,
									 unsigned long	ulAlgorithm,
									 unsigned char*	pData,				int		iDataLength,
									 unsigned char*	pHashData,			int*	piHashDataLength);

extern "C" __declspec(dllexport)
unsigned long __stdcall HashFile(unsigned long	ulModuleHandle,
							  	 unsigned long	ulSessionHandle,
								 unsigned long	ulAlgorithm,
								 unsigned char*	pData,				int		iDataLength,
								 unsigned char*	pHashData,			int*	piHashDataLength);


//////////////////////////////////////////////////////////////////
//   		   	 非對稱式加解密相關函式相關函式					//
//////////////////////////////////////////////////////////////////


extern "C" __declspec(dllexport)
unsigned long __stdcall MakeSignature(unsigned long		ulModuleHandle,
							 		  unsigned long		ulSessionHandle,
									  unsigned long		ulAlgorithm,
									  unsigned char*	pDataBeforeSign,	int		iDataBeforeSignLength,
									  unsigned long		ulPvKeyObject, 
									  unsigned char*	pSignature,			int*	piSignatureLength);

extern "C" __declspec(dllexport)
unsigned long __stdcall VerifySignature(unsigned long		ulModuleHandle,
										unsigned long		ulSessionHandle,
										unsigned long		ulAlgorithm,
										unsigned char*		pDataBeforeSign,	int		iDataBeforeSignLength,
										unsigned long		ulPubKeyObject, 
										unsigned char*		pSignature,			int		iSignatureLength);

extern "C" __declspec(dllexport)
unsigned long __stdcall PublicKeyEncryption(unsigned long	ulModuleHandle,
							  				unsigned long	ulSessionHandle,
										    unsigned long	ulAlgorithm,
											unsigned char*	pPlainData,			int		iPlainDataLength, 
											unsigned long	ulPubKeyObject, 
											unsigned char*	pCipherData,		int*	piCipherDataLength);

extern "C" __declspec(dllexport)
unsigned long __stdcall PrivateKeyDecryption(unsigned long	ulModuleHandle,
							  				 unsigned long	ulSessionHandle,
										     unsigned long	ulAlgorithm,
											 unsigned char*	pCipherData,		int		iCipherDataLength,
											 unsigned long	ulPvKeyObject,
											 unsigned char*	pPlainData,			int*	piPlainDataLength);


//////////////////////////////////////////////////////////////////
//   		   	  對稱式加解密相關函式相關函式					//
//////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)
unsigned long __stdcall GenerateSessionKey(int iLength, unsigned char* pbKey);

extern "C" __declspec(dllexport)
unsigned long __stdcall SymEncryptionAlg(unsigned long	ulModuleHandle,
							  			 unsigned long	ulSessionHandle,
										 unsigned long	ulAlgorithm,
										 unsigned char*	pPlainData,			int		iPlainDataLength,
										 unsigned char*	pIniVector,			int		iIniVectorLength,
										 unsigned long	ulSsKeyObject,
										 unsigned char*	pCipherData,		int*	piCipherDataLength);

extern "C" __declspec(dllexport)
unsigned long __stdcall SymDecryptionAlg(unsigned long	ulModuleHandle,
							  			 unsigned long	ulSessionHandle,
										 unsigned long	ulAlgorithm,
										 unsigned char*	pCipherData,		int		iCipherDataLength,
										 unsigned char*	pIniVector,			int		iIniVectorLength,
										 unsigned long	ulSsKeyObject, 
										 unsigned char*	pPlainData,			int*	piPlainDataLength);


//////////////////////////////////////////////////////////////////
//			       與過去相容但不再建議使用之函式				//
//////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)
unsigned long __stdcall OldHashFile(unsigned char* pInputData, int iInputLength, unsigned char* hashvalue, int *hashlen);

//////////////////////////////////////////////////////////////////
//						 取得詳細錯誤碼函式						//
//////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)
unsigned long __stdcall GetCryptoapiDetailError();

#endif
