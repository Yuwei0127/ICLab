#ifndef __CHTHISECURESTRUCT_DLL__
#define __CHTHISECURESTRUCT_DLL__

#define	Cert_Serial_Length			20
#define Max_GPKI_Cert_Length		4096
#define	EncryptPrivateKey_Length	2048
#define EncryptPrivateKey_PWLength	256
#define	DN_Length					256
#define	Validity_Length				32
#define	Cert_Extension_Length		1024

typedef struct CertBasicStruct
		{
			unsigned char*		pTbsCertificate;	
			int					iTbsCertificateLength;
			unsigned char*		pSignatureAlgorithm;
			int					iSignatureAlgorithmLength;
			unsigned char*		pSignature;
			int					iSignatureLength;
		}CertBasicStruct;

typedef struct CRLBasicStruct
		{
			int					iCRLType;
			unsigned char*		pTbsCRL;	
			int					iTbsCRLLength;
			unsigned char*		pSignatureAlgorithm;
			int					iSignatureAlgorithmLength;
			unsigned char*		pSignature;
			int					iSignatureLength;
		}CRLBasicStruct;

typedef struct CRLRevokedInfoStruct
		{
			unsigned char	pSerialNumber[Cert_Serial_Length];
			int				iSerialNumberLength;
			char			pRevokedDate[Validity_Length];
			int				iRevokedReason;
		}CRLRevokedInfoStruct;

typedef struct CertInfoStruct
		{
			unsigned char		ucSerialnumber[Cert_Serial_Length];	
			int					iSerialnumberLength;
			char				ucIssuerDN[DN_Length];	
			int					iIssuerDNLength;
			char				ucSubjectDN[DN_Length];	
			int					iSubjectDNLength;
			struct tm 			BeginDate;
			struct tm			EndDate;     
			unsigned char		ucExtension[Cert_Extension_Length];
			int					iExtensionLength;
		}CertInfoStruct;

typedef struct CRLInfoStruct
		{
			char				ucIssuerDN[DN_Length];	
			int					iIssuerDNLength;
			char				cCRLthisUpdate[Validity_Length];
			struct tm			CRLthisUpdate;
			char				cCRLnextUpdate[Validity_Length];
			struct tm			CRLnextUpdate;
		}CRLInfoStruct;

typedef struct CrlRequestStruct
		{
			unsigned char		ucRequestCertSerialnumber[Cert_Serial_Length];
			int					iSerialNumberLength;
			char				cCRLthisUpdate[Validity_Length];
			struct tm			CRLthisUpdate;
			char				cCRLnextUpdate[Validity_Length];
			struct tm			CRLnextUpdate;
			int					CertStatus;		//0:¦³®Ä  1:¼o¤î
			int					RevokedReason;	
			char				cRevokedDate[Validity_Length];
			struct tm			RevokedDate;
		}CrlRequestStruct;


typedef struct PrivateKeyInputData
		{
			char				sPassword[EncryptPrivateKey_PWLength];	
			int					iPasswordLength;
			unsigned char		pPrivateKeyData[EncryptPrivateKey_Length];
			int					iPrivateKeyDataLength;
		}PrivateKeyInputData;


typedef struct OCSstruct 
	{
		unsigned char	SerialNumber[Cert_Serial_Length];
		int				ResponseStatus;
		int				CertStatus;
		int				RevokeReason;
		char			PequestTime[Validity_Length];
} OCSstruct;



#endif
