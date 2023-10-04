#ifndef __GET_CERT_INF_LIB__
#define __GET_CERT_INF_LIB__

#include "chthisecurestructva.h"
 
extern "C" __declspec(dllexport)
unsigned long __stdcall GetParsingDetailError();

extern "C" __declspec(dllexport)
int __stdcall DecodeCertificate( unsigned char*		pCertificate,	int		iCertificateLength, 
								 CertBasicStruct&	SCertificate);

extern "C" __declspec(dllexport)
void __stdcall CertBasicStructDestructor(CertBasicStruct&	SCert);

extern "C" __declspec(dllexport)
int __stdcall GetSerialNumber( CertBasicStruct&	SCertificate,
							   unsigned char*	ppSerialNumber,	    int*	piSerialNumberLength);

extern "C" __declspec(dllexport)
int __stdcall GetCertIssuerDN( CertBasicStruct&	SCertificate,
							   char*			ppchDN);

extern "C" __declspec(dllexport)
int __stdcall GetCertSubjectDN( CertBasicStruct&	SCertificate,
								char*				ppchDN);

extern "C" __declspec(dllexport)
int __stdcall GetLDAPTypeCertIssuerDN( CertBasicStruct&	SCertificate,
									   char*			ppchDN);

extern "C" __declspec(dllexport)
int __stdcall GetLDAPTypeCertSubjectDN( CertBasicStruct&	SCertificate,
										char*				ppchDN);

extern "C" __declspec(dllexport)
int __stdcall GetRDNFromDN( char*	szDN,
 							char*	szRDNT,	char*	pchRDN);

extern "C" __declspec(dllexport)
int __stdcall GetCertValidity( CertBasicStruct&		CertBasicStruct, 
							   struct tm&			SnotBeforeDate, 
							   struct tm&			SnotAfterDate);

extern "C" __declspec(dllexport)
int __stdcall GetCertInfo( CertBasicStruct&	SCertificate, 
						   CertInfoStruct&	SCertInfo);

extern "C" __declspec(dllexport)
int __stdcall GetExtension( unsigned char*	pExtension,			int		iExtensionLen,
							unsigned int*	pfOID,				int		iOIDLen, 
							bool*			pbCritical,
						    unsigned char*	ppSubExtension,		int*	piSubExtensionLen);

extern "C" __declspec(dllexport)
int __stdcall GetSubjectDirectoryAttributes(unsigned char	*pucSubExtension,	int	iSubExtensionLength,
										    unsigned int	*iOID,				int OIDLen, 
											unsigned char	*ppucAttribute,		int	*piAttributeLength);


extern "C" __declspec(dllexport)
int __stdcall ExtractOIDFromDerCode(unsigned char	*pDerCodeData,	int	iDerCodeDataLength,
									char			*sOutOID);

extern "C" __declspec(dllexport)
int __stdcall VerifyCertSignature( CertBasicStruct&	SCertificate,
								   CertBasicStruct&	SCACertificate);


extern "C" __declspec(dllexport)
int __stdcall DecodeCRL( unsigned char*		pCRL,	int		iCRLLength, 
						 CRLBasicStruct&		SCRList);

extern "C" __declspec(dllexport)
int __stdcall VerifyCRLSignature( CRLBasicStruct&	SCRList,
								  CertBasicStruct&	SCACertificate);

extern "C" __declspec(dllexport)
int __stdcall SearchCRL( CrlRequestStruct&	SCRS,
						 CRLBasicStruct*		SCRList,
						 int					iCrlNumber);

extern "C" __declspec(dllexport)
void __stdcall CRLBasicStructDestructor(CRLBasicStruct&		SCRList);

//相容舊版GCA憑證廢止狀態查詢
extern "C" __declspec(dllexport)
int __stdcall SearchCRL_OLD(char			*sCRLPathName,
							unsigned int	iReqCertSerial,
							char			*pcRevokeDate , 
							int				*piReason);

//2003/12/16新增功能
extern "C" __declspec(dllexport)
int __stdcall GetCRLInfo(CRLBasicStruct&	SCRList, CRLInfoStruct&	SCRLInfo);

extern "C" __declspec(dllexport)
int __stdcall FastSearchCRL(CrlRequestStruct&	SCRS,
							CRLInfoStruct&		SCRInfo);

extern "C" __declspec(dllexport)
int __stdcall GetCRLRecord(unsigned char*		pCRL,	int		iCRLLength,
						   int					iIndex, CRLRevokedInfoStruct&	CRLRIS);

#endif



