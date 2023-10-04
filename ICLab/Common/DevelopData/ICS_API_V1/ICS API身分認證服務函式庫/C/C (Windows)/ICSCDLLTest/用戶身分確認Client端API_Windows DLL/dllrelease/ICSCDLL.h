// MOICA憑證用戶身分確認服務 DLL with Open SSL
//一般
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <errno.h>

#include <sys/types.h>
#include <winsock2.h>

#include "openssl/rsa.h"      
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/evp.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/bio.h"
//v3 用於擷取憑證之Hash值 
#include "openssl/sha.h"    //直接使用 openssl 的 SHA1 函數


/*
//HiSecure
#include "hisecure/chthisecurecryptoapiva.h"
#include "hisecure/chthisecurestructva.h"
#include "hisecure/GPKICFunction.h"
#include "hisecure/pkcs11.h"
#include "hisecure/errortable.h"
*/

//--- 設定最大的查詢筆數  -----
#define MaxRecords  1000
#define CERR_ServerResponseError	2000 //Server Get Data, but return Error Message
#define CERR_ICSResponse_NotFound	2001
#define CERR_ClientID_NotFound		2002
#define CERR_ClientID_Incorrect		2003
#define CERR_Nonce_NotFound			2004
#define CERR_Nonce_Incorrect		2005
#define CERR_ReqTime_NotFound		2006
#define CERR_TotalCount_NotFound	2007
#define CERR_TotalCount_BadFormat	2008
#define CERR_TotalCount_BadValue	2009
#define CERR_Rec_Fail				2011
#define CERR_Rec_Incorrect			2012
#define CERR_ID_Fail				2013
#define CERR_SN_Fail				2014
#define CERR_SSID4_Fail				2015
#define CERR_Code_Fail				2016
#define CERR_Code_Incorrect			2017

//--- 存放每一筆要查詢的 [憑證DN序號+身份證字號]對 資料,以及查詢結果代碼
typedef struct 
{
	char	SN[17];		//憑證DN中的序號 例如:"0000003111415257"
	//char	PID[11];	//身份證字號 例如:"Y123456789"
	//2016年7月27日修改，支援個資查詢功能(105年強化及擴充自然人憑證管理身分認證服務系統)
	char	PID[141];	//身份證字號 例如:"Q121245503<Birth>19650422</Birth><Phone>_</Phone><Cemail>_</Cemail><ACode>64000150</ACode>"
	int		_Code;		//個別查詢結果代碼, 0:正確符合, 1,2,3,4...:不正確
}SNPID;


typedef struct 
{
	char	_ClientID[101];	//v3 客戶端憑證姆紋識別碼
	char	_Nonce[16];		//封包識別15碼亂數(0~9a~zA~Z)
	char	_DateTime[15];	//封包產生時間
	int		_Result;		//批次驗證結果
	int		Count;			//批次待驗之筆數
	SNPID	*lpSNPID;		//連結至SNPID的資料結構
}ICSDATA;


//////////////////////////////////////////////////////
// MOICA憑證用戶身分確認服務應用程式介面的函數      //
//////////////////////////////////////////////////////
/*
extern "C" __declspec(dllexport) int iMake_ICSReqPKT
(
	int		iSeed,				//產生Nonce的亂數種子 MOD 1000
	char	*szClientCerFN,		//v3 客戶端憑證檔名
	ICSDATA	*lpRawICSData,		//待包裹的序號身份證號原始資料包
	int		*iRetPKTLength,		//包裹完的ICSReq封包長度
	char	**szRetPKT			//包裹完的ICSReq封包
);
依iSeed產生Nonce, 將輸入資料 lpRawICSData 內的資料包成<ICSReq>...</ICSReq> 放在 szRetPKT 輸出
<輸入參數>
	iSeed : 用來產生Nonce的randow seed
	lpRawICSData : 原始的序號身份證號資料包
		此資料包會使用到
		lpRawICSData->_ClientID //v3
		lpRawICSData->lpSNPID->SN  (共lpRawICSData->Count個)
		lpRawICSData->lpSNPID->PID (共lpRawICSData->Count個)
		執行完本函式後會將所產生的Nonce及DateTime回存於lpRawICSData內
<輸出參數>
	iRetPKTLength : 所產生的ICSReq封包長度
	szRetPKT : 所產生的ICSReq封包內容
<傳回值>
	 0: 正確處理完成
	-1: 記憶體配置失敗

ICSReq封包範例:
<ICSReq>
<ClientID>4FC83967753F21CD1A7DD34D885CF04AB3573612</ClientID>
<Nonce>Ae34j5F6E34DE78</Nonce>
<ReqTime>20050124153347</ReqTime><TotalCount>2</TotalCount>
<Rec><ID>1</ID><SN>000000010000003</SN><SSID>N100939455</SSID></Rec>
<Rec><ID>1</ID><SN>000000010000003</SN><SSID>N100939455</SSID></Rec>
</ICSReq>
*/
extern "C" __declspec(dllexport) int iMake_ICSReqPKT
(
	int		iSeed,				//產生Nonce的亂數種子 MOD 1000
	char	*szClientCerFN,		//v3 客戶端憑證檔名
	ICSDATA	*lpRawICSData,		//待包裹的序號身份證號原始資料包
	int		*iRetPKTLength,		//包裹完的ICSReq封包長度
	char	**szRetPKT			//包裹完的ICSReq封包
);





/*
extern "C" __declspec(dllexport) int iMake_ICSRequestPKT
(
	int		iICSReqLength,		//ICSReq封包長度
	char	*szICSReq,			//ICSReq封包
	int		iSignatureLength,	//ICSReq封包的簽章長度
	char	*szSignature,		//ICSReq封包
	int		*iRetPKTLength,		//回應的ICSRequest封包長度
	char	**szRetPKT			//回應的ICSRequest封包
);
將輸入資料szICSReq 內的資料包及簽過後的簽章szSignature, 
包成最終的<ICSRequest>....</ICSRequest>封包放在 szRetPKT 輸出
<輸入參數>
	iICSReqLength : ICSReq封包長度
	szICSReq : 先前所產生的ICSReq封包內容
	iSignatureLength : 簽章長度
	szSignature : 簽章
	
<輸出參數>
	iRetPKTLength : 最終所產生的ICSReqest封包長度
	szRetPKT : 最終所產生的ICSReqest封包內容
<傳回值>
	 0: 正確處理完成
	-1: 記憶體配置失敗

ICSRequest封包範例
<ICSRequest>
封包ICSReq的資料
<ICSReqLength>封包ICSReq的資料長度</ICSReqLength>
<Signature>封包ICSReq的簽章</Signature>
</ICSRequest>
*/
extern "C" __declspec(dllexport) int iMake_ICSRequestPKT
(
	int		iICSReqLength,		//ICSReq封包長度
	char	*szICSReq,			//ICSReq封包
	int		iSignatureLength,	//ICSReq封包的簽章長度
	char	*szSignature,		//ICSReq封包
	int		*iRetPKTLength,		//回應的ICSRequest封包長度
	char	**szRetPKT			//回應的ICSRequest封包
);




// Open SSL應用程式介面函式
/*
extern "C" __declspec(dllexport) int iSSL_SendThenRecv
(
	char	*szIP,				//指定Server端的IP Address
	int		iPort,				//指定Server端的Port : 一般為443
	char	*szTrustCACertFN,	//指定簽發Server憑證之CA (Certificate)
	char	*szCGIURL,			//Server端CGI的執行路徑
	int		iToBeSendData,		//待送資料長度
	char	*szToBeSendData,	//待送資料
	int		*iRetPKTLength,		//Server端回傳之資料長度
	char	**szRetPKT			//Server端回傳之資料
);
與Server建立SSL連線並將待送之資料傳至Server, 並接受Server端之回應封包
<輸入參數>
	szIP			:指定Server端的IP Address
	iPort			:指定Server端的Port : 一般為443
	szTrustCACertFN	:指定簽發Server憑證之CA (Certificate)
	szCGIURL		:指定Server端CGI的執行路徑
	iToBeSendData	:待送資料長度
	szToBeSendData	:待送資料
	
<輸出參數>
	iRetPKTLength	:Server端回傳之資料長度
	szRetPKT		:Server端回傳之資料

<傳回值>
	 0: 正確處理完成
	-1: 記憶體配置失敗
	-2: 起始WinSock錯誤
	-3: WinSock版本
	-4: 起始SSL錯誤
	-5: 配置SSL使用之變數失敗
	-6: 取得CA憑證位置失敗(若需要驗證的話)
	-7: 配置Socket失敗
	-8: 轉換網路位址失敗
	-9: 連線失敗
	-10: 配置SSL使用之變數失敗
	-11: 設定Socket給SSL失敗
	-12: SSL連線失敗
	-13: 得到Server端的憑證失敗
	-14: 擷取Server憑證之SubjectName失敗
	-15: 擷取Server憑證之IssuerName失敗
	-16: 無法傳送待送資料至Server端
*/
extern "C" __declspec(dllexport) int iSSL_SendThenRecv
(
	char	*szIP,				//指定Server端的IP Address
	int		iPort,				//指定Server端的Port : 一般為443
	char	*szTrustCACertFN,	//指定簽發Server憑證之CA (Certificate)
	char	*szCGIURL,			//Server端CGI的執行路徑
	int		iToBeSendData,		//待送資料長度
	char	*szToBeSendData,	//待送資料
	int		*iRetPKTLength,		//Server端回傳之資料長度
	char	**szRetPKT			//Server端回傳之資料
);






/*
extern "C" __declspec(dllexport) int iParse_ICSResponsePKT
(
	ICSDATA		*lpRawICSData,		//回存解析封包後之查驗結果
	int			iResponsePKTLength,	//Server回應之封包長度
	char		*szResponsePKT		//Server回應之封包
);
解析Servre回傳的<ICSResponse>....</ICSResponse>封包 取出查驗結果放於lpRawICSData->lpSNPID->Code
<輸入參數>
	iResponsePKTLength : Server回傳的ICSResponse封包長度
	szResponsePKT : Server回傳的ICSResponse封包內容
<輸出參數>
	lpRawICSData : 將查驗結果回存於此
		執行完本函式後
		case 1 若Server端正確處理()
				--> lpRawICSData->_Result=0; 且將查驗結果回存於lpRawICSData->lpSNPID->_Code
		case 2 若Server端未能正確處理
				--> lpRawICSData->_Result=Server回傳的錯誤值, lpRawICSData->lpSNPID->_Code不具參考意義

<傳回值> 0, -1, 2000, 2001~2017
	0    : 本函式正確處理完成, 查驗結果存放於 lpRawICSData->lpSNPID->_Code
			Server回應之查詢結果代碼解說如下:	
				0: 表示此筆SerialNumber已有申請MOICA憑證，且與client端所列的身份證字號相符合，
				   而且該MOICA憑證已經開卡(有效或停用)，這是一般符合需求的狀態
				1: 表示此筆(SerialNumber+身份證字號)資料對，資料庫中沒有對應的紀錄
				2: 表示此身份證字號，沒有對應的憑證
				3: 表示此筆SerialNumber已申請MOICA憑證，且與client端所列的身份證字號相符合，但是該MOICA憑證尚未開卡
				//4: 表示此筆SerialNumber已申請MOICA憑證，且與client端所列的身份證字號相符合，但是該MOICA憑證已過期或已廢止
				4: 表示此筆SerialNumber已申請MOICA憑證，且與client端所列的身份證字號相符合，但是該MOICA憑證已過期
				5: 表示此筆SerialNumber已申請MOICA憑證，且與client端所列的身份證字號相符合，但是該MOICA憑證已廢止
				(4,5可能不實際區分出來)
				如果身份確認結果所傳回的整數值不屬上述範圍，且大於1000，即是系統處理時發生錯誤所產生的錯誤代碼。

	-1   : Server未收到資料, 其WebServer回應錯誤或接收到非Http封包格式
	2000 : Server已收到資料但回應錯誤訊息, 
			該錯誤訊息存放於lpRawICSData->_Result, 代碼如下:
				ERR_NoCGIParam             1001 //輸入之CGI參數為空白,沒有值
				ERR_ICSRequest_NotFound    1002 //取出 <ICSRequest> 參數失敗
				ERR_ICSReq_NotFound        1003 //取出 <ICSReq> 參數失敗
				ERR_ClientID_NotFound      1004 //取出 <ClientID> 參數失敗
				ERR_Nonce_NotFound         1005 //取出 <Nonce> 參數失敗
				ERR_ReqTime_NotFound       1006 //取出 <ReqTime> 參數失敗
				ERR_TotalCount_NotFound    1007 //取出 <TotalCount> 參數失敗
				ERR_TotalCount_BadFormat   1008 //參數 <TotalCount> 格式錯誤:非數值
				ERR_TotalCount_BadValue    1009 //參數 <TotalCount> 格式錯誤:數值超過範圍
				ERR_ICSReqLength_NotFound  1011 //取出 <ICSReqLength> 參數失敗
				ERR_ICSReqLength_BadFormat 1012 //參數 <ICSReqLength> 格式錯誤:非數值
				ERR_ICSReqLength_NotMatch  1013 //參數 <ICSReqLength> 的值和<ICSReq>的長度不同
				ERR_Signature_NotFound     1014 //取出 <Signature> 參數失敗
				ERR_Signature_BadFormat    1015 //參數 <Signature> 格式錯誤,無法從HEX格式轉成binary值
				ERR_Rec_n_Fail             -10000   //取出第 n 行 <Rec> 參數失敗,或TotalCount值不正確
				ERR_ID_n_Fail              -20000  //取出第 n 個 <ID> 參數失敗,或資料錯誤,或不是流水號
				ERR_SN_n_Fail              -30000   //取出第 n 個 <SN> 參數失敗,或資料錯誤
				ERR_SSID_n_Fail            -40000   //取出第 n 個 <SSID>參數失敗,或資料錯誤
				ERR_ClientID_Incorrect     1020 //系統不認可的ClientID,系統無此ClientID的資訊
				ERR_Verify_Signature_Fail  1021 //檢驗client端所做的簽章失敗
				ERR_ConnectDB_Fail         1022 //連線到資料庫失敗

	2001 ~ 2017 : Client 收到回應資料但解析封包時出錯, 錯誤代碼如下:
			CERR_ICSResponse_NotFound	2001
			CERR_ClientID_NotFound		2002
			CERR_ClientID_Incorrect		2003
			CERR_Nonce_NotFound			2004
			CERR_Nonce_Incorrect		2005
			CERR_ReqTime_NotFound		2006
			CERR_TotalCount_NotFound	2007
			CERR_TotalCount_BadFormat	2008
			CERR_TotalCount_BadValue	2009
			CERR_Rec_Fail				2011
			CERR_Rec_Incorrect			2012
			CERR_ID_Fail				2013
			CERR_SN_Fail				2014
			CERR_SSID4_Fail				2015
			CERR_Code_Fail				2016
			CERR_Code_Incorrect			2017
*/
extern "C" __declspec(dllexport) int iParse_ICSResponsePKT
(
	ICSDATA		*lpRawICSData,		//回存解析封包後之查驗結果
	int			iResponsePKTLength,	//Server回應之封包長度
	char		*szResponsePKT		//Server回應之封包
);

// 提供Free的Function 
extern "C" __declspec(dllexport) void vFreeData (char *szData);

//將Binary轉成hex 將二進位簽章值轉成16進位時用到
extern "C" __declspec(dllexport) int bin2hex(unsigned char *input, int length, char *output);

