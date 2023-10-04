#ifndef __NET_FUNC__
#define __NET_FUNC__

#include "chthisecurestructva.h"


extern "C" __declspec(dllexport)
int __stdcall BuildTobesignedOCSPRequest(OCSstruct		*OCSp,						int num,
										 unsigned char	*pNounce,					int	iNounceLength,
										 unsigned char	*RequestIssueCertificate,	
										 unsigned char	*ucppTobesignedOCSPReq,	int	*iTobesignedOCSPReqLength);

extern "C" __declspec(dllexport)
int __stdcall QueryOCSfromOCSPRequest(OCSstruct *OCSp,						int num,
											unsigned char *ucpTobesignedOCSPReq,	int	iTobesignedOCSPReqLength,
											unsigned char *TbsRequestSignature,	int iTbsRequestSignatureLength,
											unsigned char *RequestIssueCertificate,
											unsigned char *SenderCertificate,
											unsigned char *OCSPServerCertificate,
											char *ServerURL,						
											char *ProxyName,						int ProxyPort);



#endif