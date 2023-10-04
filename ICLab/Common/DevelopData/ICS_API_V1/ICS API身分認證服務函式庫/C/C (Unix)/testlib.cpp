#include "ICSCLib.h"

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///             ID Certificate Check with OpenSSL                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// 用來讀取 private key 的函式 
EVP_PKEY * LoadPrivateKey(const char *filename)
{
	BIO			*bp;
	EVP_PKEY	*pkey;

	// 初始化 BIO，設定為「檔案操作」 
	if ((bp = BIO_new(BIO_s_file())) == NULL) 
	{
		//ERR_print_errors_fp(stderr);
		return (NULL);
	}
	
	// 設定讀取的檔案名稱 
	BIO_read_filename(bp, filename);

	// 讀取 private key 
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
	const char	*keyfile,		//密鑰檔名
	const char	*ToBeSigned,	//待簽之訊息
	const int	tbs_len,		//待簽之訊息長度
	char		*signHEX,		//簽章結果(HEX格式)
	int			*signHex_len	//簽章長度
)
{
    EVP_MD_CTX  sign_ctx;
    EVP_PKEY    *pkey;
	unsigned char signature[1025];
	int signlen;

    // 載入所有的演算法
    OpenSSL_add_all_algorithms();

    // 載入所有的錯誤訊息，因為接下來可能會用到 ERR_XXXXX() 函式印出
    // OpenSSL 函式庫相關的錯誤訊息。
    ERR_load_ERR_strings();
    ERR_load_crypto_strings();

    // 呼叫 private key 讀取函式，
    if ((pkey = LoadPrivateKey(keyfile)) == NULL) return -111;

    // 初始化，選擇 SHA1 + RSA 為簽章演算法 
    EVP_SignInit(&sign_ctx, EVP_sha1());

    // 簽章
    EVP_SignUpdate(&sign_ctx, ToBeSigned, tbs_len);

    // 產生簽章結果，存在 md_value 中，長度存於 md_len 
    EVP_SignFinal(&sign_ctx, signature, (unsigned int *)(&signlen), pkey);

	//轉成HEX格式
	bin2hex(signature, signlen, signHEX);
	(*signHex_len)=signlen*2;

	return 0; //SUCCESS
}

int iIDCertCheck_OpenSSL
(
	ICSDATA		*lpICSData,				//身份證號, 憑證序號...等原始資料
										//及查驗結果介面封包(詳見ICSCLib.h)
	int			iSeed,					//產生封包Nonce所要之亂數種子
	char		*szIP,					//指定Server端的IP Address
	int			iPort,					//指定Server端的Port : 一般為443
	char		*szClientCertFN,		//v3 指定Client端的憑證檔
	char		*szTrustCACertFN,		//指定簽發Server憑證之CA (Certificate)
	const char	*szPrivateKeyFN,		//客戶端私密金鑰檔之檔名
	char		*szCGIURL,				//Server端CGI的執行路徑
	int			*iRequestPKTLength,		//本函式包成的Request封包長度
	char		**szRequestPKT,			//本函式包成的Request封包
	int			*iResponsePKTLength,	//本函式收到的Response封包長度
	char		**szResponsePKT,		//本函式收到的Response封包
	int			*iErrorCode				//若期間發生錯誤, 會設定ErrorCode
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
		//使用HiSecure or OpenSSL
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
	else //OpenSSL簽章測試
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
		strcat(szOut,"伺服端回應碼(lpICSData->_Result)對照表\n");
		strcat(szOut,"1001 輸入之CGI參數為空白,沒有值\n");
		strcat(szOut,"1002 取出 <ICSRequest> 參數失敗\n");
		strcat(szOut,"1003 取出 <ICSReq> 參數失敗\n");
		strcat(szOut,"1004 取出 <ClientID> 參數失敗\n");
		strcat(szOut,"1005 取出 <Nonce> 參數失敗\n");
		strcat(szOut,"1006 取出 <ReqTime> 參數失敗\n");
		strcat(szOut,"1007 取出 <TotalCount> 參數失敗\n");
		strcat(szOut,"1008 參數 <TotalCount> 格式錯誤:非數值\n");
		strcat(szOut,"1009 參數 <TotalCount> 格式錯誤:數值超過範圍\n");
		strcat(szOut,"1011 取出 <ICSReqLength> 參數失敗\n");
		strcat(szOut,"1012 參數 <ICSReqLength> 格式錯誤:非數值\n");
		strcat(szOut,"1013 參數 <ICSReqLength> 的值和<ICSReq>的長度不同\n");
		strcat(szOut,"1014 取出 <Signature> 參數失敗\n");
		strcat(szOut,"1015 參數 <Signature> 格式錯誤,無法從HEX格式轉成binary值\n");
		strcat(szOut,"-10000 取出第 n 行 <Rec> 參數失敗,或TotalCount值不正確\n");
		strcat(szOut,"-20000 取出第 n 個 <ID> 參數失敗,或資料錯誤,或不是流水號\n");
		strcat(szOut,"-30000 取出第 n 個 <SN> 參數失敗,或資料錯誤\n");
		strcat(szOut,"-40000 取出第 n 個 <SSID>參數失敗,或資料錯誤\n");
		strcat(szOut,"1020 系統不認可的ClientID,系統無此ClientID的資訊\n");
		strcat(szOut,"1021 檢驗client端所做的簽章失敗\n");
		strcat(szOut,"1022 連線到資料庫失敗\n");

		if (iRet!=-999) printf("%s\n", szOut);
		//put your error processing code here...

	}
	else
	{
		//sucess
		sprintf(szOut, "\n\n==========================================\n查詢封包(Request Packet)內容如下:\n==========================================\n%s", szRequest);
		printf("%s\n", szOut);

		sprintf(szOut, "\n\n==========================================\n回應封包(Response Packet)內容如下:\n==========================================\n%s", szResponse);
		printf("%s\n", szOut);
		sprintf(szOut, "\n\n==========================================\n解析結果如下:\n==========================================\nClient ID:%s\nNonce:%s\nDateTime:%s\nResult:%d\nCount:%d\n",
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





		
