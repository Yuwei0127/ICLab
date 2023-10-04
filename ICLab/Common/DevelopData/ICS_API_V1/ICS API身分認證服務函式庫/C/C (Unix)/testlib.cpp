#include "ICSCLib.h"

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///             ID Certificate Check with OpenSSL                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// �Ψ�Ū�� private key ���禡 
EVP_PKEY * LoadPrivateKey(const char *filename)
{
	BIO			*bp;
	EVP_PKEY	*pkey;

	// ��l�� BIO�A�]�w���u�ɮ׾ާ@�v 
	if ((bp = BIO_new(BIO_s_file())) == NULL) 
	{
		//ERR_print_errors_fp(stderr);
		return (NULL);
	}
	
	// �]�wŪ�����ɮצW�� 
	BIO_read_filename(bp, filename);

	// Ū�� private key 
	if ((pkey = PEM_read_bio_PrivateKey(bp, NULL, NULL, NULL))  == NULL)
	{
		//ERR_print_errors_fp(stderr);
		BIO_free(bp);
		return (NULL);
	}
	BIO_free(bp);
	return (pkey);

}//LoadPrivateKey()


// Signature by OpenSSL function
int Sign_with_OpenSSL
(
	const char	*keyfile,		//�K�_�ɦW
	const char	*ToBeSigned,	//��ñ���T��
	const int	tbs_len,		//��ñ���T������
	char		*signHEX,		//ñ�����G(HEX�榡)
	int			*signHex_len	//ñ������
)
{
    EVP_MD_CTX  sign_ctx;
    EVP_PKEY    *pkey;
	unsigned char signature[1025];
	int signlen;

    // ���J�Ҧ����t��k
    OpenSSL_add_all_algorithms();

    // ���J�Ҧ������~�T���A�]�����U�ӥi��|�Ψ� ERR_XXXXX() �禡�L�X
    // OpenSSL �禡�w���������~�T���C
    ERR_load_ERR_strings();
    ERR_load_crypto_strings();

    // �I�s private key Ū���禡�A
    if ((pkey = LoadPrivateKey(keyfile)) == NULL) return -111;

    // ��l�ơA��� SHA1 + RSA ��ñ���t��k 
    EVP_SignInit(&sign_ctx, EVP_sha1());

    // ñ��
    EVP_SignUpdate(&sign_ctx, ToBeSigned, tbs_len);

    // ����ñ�����G�A�s�b md_value ���A���צs�� md_len 
    EVP_SignFinal(&sign_ctx, signature, (unsigned int *)(&signlen), pkey);

	//�নHEX�榡
	bin2hex(signature, signlen, signHEX);
	(*signHex_len)=signlen*2;

	return 0; //SUCCESS
}

int iIDCertCheck_OpenSSL
(
	ICSDATA		*lpICSData,				//�����Ҹ�, ���ҧǸ�...����l���
										//�άd�絲�G�����ʥ](�Ԩ�ICSCLib.h)
	int			iSeed,					//���ͫʥ]Nonce�ҭn���üƺؤl
	char		*szIP,					//���wServer�ݪ�IP Address
	int			iPort,					//���wServer�ݪ�Port : �@�묰443
	char		*szClientCertFN,		//v3 ���wClient�ݪ�������
	char		*szTrustCACertFN,		//���wñ�oServer���Ҥ�CA (Certificate)
	const char	*szPrivateKeyFN,		//�Ȥ�ݨp�K���_�ɤ��ɦW
	char		*szCGIURL,				//Server��CGI��������|
	int			*iRequestPKTLength,		//���禡�]����Request�ʥ]����
	char		**szRequestPKT,			//���禡�]����Request�ʥ]
	int			*iResponsePKTLength,	//���禡���쪺Response�ʥ]����
	char		**szResponsePKT,		//���禡���쪺Response�ʥ]
	int			*iErrorCode				//�Y�����o�Ϳ��~, �|�]�wErrorCode
)
{
	int		iRet;
	int		iSignature=0, iICSReq=0, iICSRequest=0, iICSResponse=0;
	char	*szSignature=NULL, *szICSReq=NULL, *szICSRequest=NULL, *szICSResponse=NULL;

	*iRequestPKTLength=0;
	*szRequestPKT=NULL;
	*iResponsePKTLength=0;
	*szResponsePKT=NULL;
	*iErrorCode=0;
	
	//v3
	iRet=iMake_ICSReqPKT( iSeed, szClientCertFN, lpICSData , &iICSReq, &szICSReq);
	if (iRet!=0) return -810; //Memory Allocation Failure : szICSReq

	iSignature=0;
	szSignature = (char *)malloc(4096*sizeof(char));
	if (szSignature==NULL)
	{
		vFreeData(szICSReq);
		szICSReq=NULL;
		return -820;
	}

	iRet=Sign_with_OpenSSL(szPrivateKeyFN, szICSReq, iICSReq, szSignature, &iSignature);
	if(iRet!=0)
	{
		//pem key sign error, error code: iRet
		vFreeData(szICSReq);
		free(szSignature); szSignature=NULL;
		*iErrorCode=iRet;
		return -820;
	}

	iRet=iMake_ICSRequestPKT(iICSReq, szICSReq, iSignature, szSignature, &iICSRequest, &szICSRequest);
	if (iRet!=0) 
	{
		//Memory Allocation Failure : szICSRequestPKT
		vFreeData(szICSReq);
		free(szSignature); szSignature=NULL;
		return -830;
	}

	vFreeData(szICSReq);
	free(szSignature); szSignature=NULL;

	*iRequestPKTLength=iICSRequest;
	*szRequestPKT=szICSRequest;

	iRet=iSSL_SendThenRecv
		(
			szIP,
			iPort,
			szTrustCACertFN,
			szCGIURL,
			iICSRequest,
			szICSRequest,
			&iICSResponse,
			&szICSResponse
		);

	if (iRet!=0)
	{
		*iErrorCode=iRet;
		return -840;
	}

	*iResponsePKTLength=iICSResponse;
	*szResponsePKT=szICSResponse;

	iRet = iParse_ICSResponsePKT( lpICSData, iICSResponse, szICSResponse);
	if (iRet!=0)
	{
		*iErrorCode=iRet;
		return -850;
	}

	return 0;
}



int main()
{
	int		iRet, iErrorCode, i;
	ICSDATA	*lpICSData=NULL;
	int		iRequest=0, iResponse=0;
	char	*szRequest=NULL, *szResponse=NULL;
	FILE	*StatusFile;
	char	szDataBuf[256], szIP[256], szTrustCACert[256], szCGIURL[256], szKeyFile[256], szPassWord[256], szClientCertFile[256]; //v3
	int		iPort;
	bool	iChoiceHiSecure;
	char	szTmp[100], szOut[50000];


	StatusFile = fopen( "ICSCTest.txt", "r+t" );
	if( StatusFile == NULL )
	{
		iChoiceHiSecure=true;
		strcpy(szIP, "10.144.133.180");
		iPort=443;
		strcpy(szTrustCACert,"GTestCA.pem");
		strcpy(szCGIURL, "/cgi-bin/CheckSNPID/CheckSNPID");
		strcpy(szClientCertFile,"cert3045.cer"); //v3
		strcpy(szKeyFile, "key3045.pem");
		//strcpy(szPassWord, "12345678");
	}
	else
	{
		//�ϥ�HiSecure or OpenSSL
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		if ( (szDataBuf[0]=='H') || (szDataBuf[0]=='h') ) iChoiceHiSecure=true; else iChoiceHiSecure=false;

		//IP
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		strcpy(szIP, szDataBuf);

		//Port
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		iPort=atoi(szDataBuf);

		//ServerCA
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		strcpy(szTrustCACert, szDataBuf);


		//CGIURL
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		strcpy(szCGIURL, szDataBuf);

		//v3 Client Certificate Assignment
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf); 
		strcpy(szClientCertFile, szDataBuf);

		//Key File
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		strcpy(szKeyFile, szDataBuf);

		//PassWord
		//fscanf( StatusFile, "%s", szDataBuf);
		//fscanf( StatusFile, "%s", szDataBuf);
		//strcpy(szPassWord, szDataBuf);

		fclose(StatusFile);
	}


	//Ready Data
	lpICSData=(ICSDATA *) malloc(sizeof(ICSDATA));
	if (lpICSData==NULL ) { } // Memory allocate failure 
	lpICSData->Count=10;
	lpICSData->lpSNPID = (SNPID *)malloc(lpICSData->Count*sizeof(SNPID));
	if (lpICSData->lpSNPID==NULL ) { } // Memory allocate failure

	// v3 strcpy(lpICSData->ClientID, szID);
	strcpy( (lpICSData->lpSNPID+0)->SN,  "0000000010000003" );
	strcpy( (lpICSData->lpSNPID+0)->PID, "N100939455");
	strcpy( (lpICSData->lpSNPID+1)->SN,  "0000000010000013" );
	strcpy( (lpICSData->lpSNPID+1)->PID, "N100939454");
	strcpy( (lpICSData->lpSNPID+2)->SN,  "0000000010000098" );
	strcpy( (lpICSData->lpSNPID+2)->PID, "A123456781");
	strcpy( (lpICSData->lpSNPID+3)->SN,  "0000000010000774" );
	strcpy( (lpICSData->lpSNPID+3)->PID, "E200213219");
	strcpy( (lpICSData->lpSNPID+4)->SN,  "0000000010001046" );
	strcpy( (lpICSData->lpSNPID+4)->PID, "A100000065");
	strcpy( (lpICSData->lpSNPID+5)->SN,  "0000000010001047" );
	strcpy( (lpICSData->lpSNPID+5)->PID, "A120861199");
	strcpy( (lpICSData->lpSNPID+6)->SN,  "0000003111415251" );
	strcpy( (lpICSData->lpSNPID+6)->PID, "Y123456781");
	strcpy( (lpICSData->lpSNPID+7)->SN,  "0000003111415252" );
	strcpy( (lpICSData->lpSNPID+7)->PID, "Y123456782");
	strcpy( (lpICSData->lpSNPID+8)->SN,  "0000003111415253" );
	strcpy( (lpICSData->lpSNPID+8)->PID, "Y123456783");
	strcpy( (lpICSData->lpSNPID+9)->SN,  "0000003111415254" );
	strcpy( (lpICSData->lpSNPID+9)->PID, "Y123456784");


	if (iChoiceHiSecure)
	{
		printf("\n\nNo HiSecure Signature function\n");
		iRet=-999;
		/*
		iRet=iIDCertCheck_HiSecure
			(
				lpICSData,
				100,
				szIP,
				iPort,
				szClientCertFile, //v3
				szTrustCACert,
				szKeyFile,
				szPassWord,
				strlen(szPassWord),
				szCGIURL,
				&iRequest,
				&szRequest,
				&iResponse,
				&szResponse,
				&iErrorCode
			);
		*/
	}
	else //OpenSSLñ������
	{
		iRet=iIDCertCheck_OpenSSL
			(
				lpICSData,
				100,
				szIP,
				iPort,
				szClientCertFile, //v3
				szTrustCACert,
				szKeyFile,
				szCGIURL,
				&iRequest,
				&szRequest,
				&iResponse,
				&szResponse,
				&iErrorCode
			);
	}


	if (iRet!=0) // error
	{
		sprintf(szOut,"Error!!\nFunction Return: %d\nErrorCode: %d\nServer Return: %d\n", iRet, iErrorCode, lpICSData->_Result);
		strcat(szOut,"���A�ݦ^���X(lpICSData->_Result)��Ӫ�\n");
		strcat(szOut,"1001 ��J��CGI�ѼƬ��ť�,�S����\n");
		strcat(szOut,"1002 ���X <ICSRequest> �Ѽƥ���\n");
		strcat(szOut,"1003 ���X <ICSReq> �Ѽƥ���\n");
		strcat(szOut,"1004 ���X <ClientID> �Ѽƥ���\n");
		strcat(szOut,"1005 ���X <Nonce> �Ѽƥ���\n");
		strcat(szOut,"1006 ���X <ReqTime> �Ѽƥ���\n");
		strcat(szOut,"1007 ���X <TotalCount> �Ѽƥ���\n");
		strcat(szOut,"1008 �Ѽ� <TotalCount> �榡���~:�D�ƭ�\n");
		strcat(szOut,"1009 �Ѽ� <TotalCount> �榡���~:�ƭȶW�L�d��\n");
		strcat(szOut,"1011 ���X <ICSReqLength> �Ѽƥ���\n");
		strcat(szOut,"1012 �Ѽ� <ICSReqLength> �榡���~:�D�ƭ�\n");
		strcat(szOut,"1013 �Ѽ� <ICSReqLength> ���ȩM<ICSReq>�����פ��P\n");
		strcat(szOut,"1014 ���X <Signature> �Ѽƥ���\n");
		strcat(szOut,"1015 �Ѽ� <Signature> �榡���~,�L�k�qHEX�榡�নbinary��\n");
		strcat(szOut,"-10000 ���X�� n �� <Rec> �Ѽƥ���,��TotalCount�Ȥ����T\n");
		strcat(szOut,"-20000 ���X�� n �� <ID> �Ѽƥ���,�θ�ƿ��~,�Τ��O�y����\n");
		strcat(szOut,"-30000 ���X�� n �� <SN> �Ѽƥ���,�θ�ƿ��~\n");
		strcat(szOut,"-40000 ���X�� n �� <SSID>�Ѽƥ���,�θ�ƿ��~\n");
		strcat(szOut,"1020 �t�Τ��{�i��ClientID,�t�εL��ClientID����T\n");
		strcat(szOut,"1021 ����client�ݩҰ���ñ������\n");
		strcat(szOut,"1022 �s�u���Ʈw����\n");

		if (iRet!=-999) printf("%s\n", szOut);
		//put your error processing code here...

	}
	else
	{
		//sucess
		sprintf(szOut, "\n\n==========================================\n�d�߫ʥ](Request Packet)���e�p�U:\n==========================================\n%s", szRequest);
		printf("%s\n", szOut);

		sprintf(szOut, "\n\n==========================================\n�^���ʥ](Response Packet)���e�p�U:\n==========================================\n%s", szResponse);
		printf("%s\n", szOut);
		sprintf(szOut, "\n\n==========================================\n�ѪR���G�p�U:\n==========================================\nClient ID:%s\nNonce:%s\nDateTime:%s\nResult:%d\nCount:%d\n",
						lpICSData->_ClientID, //v3
						lpICSData->_Nonce, 
						lpICSData->_DateTime,
						lpICSData->_Result,
						lpICSData->Count
				);
		for (i=1;i<=10;i++)
		{
			sprintf(szTmp,"SN:%s     PID:%s     Code:%d\n", 
							(lpICSData->lpSNPID+i-1)->SN,
							(lpICSData->lpSNPID+i-1)->PID,
							(lpICSData->lpSNPID+i-1)->_Code );
			strcat(szOut,szTmp);
		}
		printf("%s\n", szOut);	

	}
	vFreeData(szRequest);
	vFreeData(szResponse);
	if (lpICSData->lpSNPID!=NULL) { free(lpICSData->lpSNPID); lpICSData->lpSNPID=NULL; }
	if (lpICSData!=NULL) { free(lpICSData); lpICSData=NULL; }


}





		
