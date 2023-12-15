// MOICA���ҥΤᨭ���T�{�A�� DLL with Open SSL
//�@��
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
//v3 �Ω��^�����Ҥ�Hash�� 
#include "openssl/sha.h"    //�����ϥ� openssl �� SHA1 ���


/*
//HiSecure
#include "hisecure/chthisecurecryptoapiva.h"
#include "hisecure/chthisecurestructva.h"
#include "hisecure/GPKICFunction.h"
#include "hisecure/pkcs11.h"
#include "hisecure/errortable.h"
*/

//--- �]�w�̤j���d�ߵ���  -----
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

//--- �s��C�@���n�d�ߪ� [����DN�Ǹ�+�����Ҧr��]�� ���,�H�άd�ߵ��G�N�X
typedef struct 
{
	char	SN[17];		//����DN�����Ǹ� �Ҧp:"0000003111415257"
	//char	PID[11];	//�����Ҧr�� �Ҧp:"Y123456789"
	//2016�~7��27��ק�A�䴩�Ӹ�d�ߥ\��(105�~�j�Ƥ��X�R�۵M�H���Һ޲z�����{�ҪA�Ȩt��)
	char	PID[141];	//�����Ҧr�� �Ҧp:"Q121245503<Birth>19650422</Birth><Phone>_</Phone><Cemail>_</Cemail><ACode>64000150</ACode>"
	int		_Code;		//�ӧO�d�ߵ��G�N�X, 0:���T�ŦX, 1,2,3,4...:�����T
}SNPID;


typedef struct 
{
	char	_ClientID[101];	//v3 �Ȥ�ݾ��ҩi���ѧO�X
	char	_Nonce[16];		//�ʥ]�ѧO15�X�ü�(0~9a~zA~Z)
	char	_DateTime[15];	//�ʥ]���ͮɶ�
	int		_Result;		//�妸���ҵ��G
	int		Count;			//�妸���礧����
	SNPID	*lpSNPID;		//�s����SNPID����Ƶ��c
}ICSDATA;


//////////////////////////////////////////////////////
// MOICA���ҥΤᨭ���T�{�A�����ε{�����������      //
//////////////////////////////////////////////////////
/*
extern "C" __declspec(dllexport) int iMake_ICSReqPKT
(
	int		iSeed,				//����Nonce���üƺؤl MOD 1000
	char	*szClientCerFN,		//v3 �Ȥ�ݾ����ɦW
	ICSDATA	*lpRawICSData,		//�ݥ]�q���Ǹ������Ҹ���l��ƥ]
	int		*iRetPKTLength,		//�]�q����ICSReq�ʥ]����
	char	**szRetPKT			//�]�q����ICSReq�ʥ]
);
��iSeed����Nonce, �N��J��� lpRawICSData ������ƥ]��<ICSReq>...</ICSReq> ��b szRetPKT ��X
<��J�Ѽ�>
	iSeed : �ΨӲ���Nonce��randow seed
	lpRawICSData : ��l���Ǹ������Ҹ���ƥ]
		����ƥ]�|�ϥΨ�
		lpRawICSData->_ClientID //v3
		lpRawICSData->lpSNPID->SN  (�@lpRawICSData->Count��)
		lpRawICSData->lpSNPID->PID (�@lpRawICSData->Count��)
		���槹���禡��|�N�Ҳ��ͪ�Nonce��DateTime�^�s��lpRawICSData��
<��X�Ѽ�>
	iRetPKTLength : �Ҳ��ͪ�ICSReq�ʥ]����
	szRetPKT : �Ҳ��ͪ�ICSReq�ʥ]���e
<�Ǧ^��>
	 0: ���T�B�z����
	-1: �O����t�m����

ICSReq�ʥ]�d��:
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
	int		iSeed,				//����Nonce���üƺؤl MOD 1000
	char	*szClientCerFN,		//v3 �Ȥ�ݾ����ɦW
	ICSDATA	*lpRawICSData,		//�ݥ]�q���Ǹ������Ҹ���l��ƥ]
	int		*iRetPKTLength,		//�]�q����ICSReq�ʥ]����
	char	**szRetPKT			//�]�q����ICSReq�ʥ]
);





/*
extern "C" __declspec(dllexport) int iMake_ICSRequestPKT
(
	int		iICSReqLength,		//ICSReq�ʥ]����
	char	*szICSReq,			//ICSReq�ʥ]
	int		iSignatureLength,	//ICSReq�ʥ]��ñ������
	char	*szSignature,		//ICSReq�ʥ]
	int		*iRetPKTLength,		//�^����ICSRequest�ʥ]����
	char	**szRetPKT			//�^����ICSRequest�ʥ]
);
�N��J���szICSReq ������ƥ]��ñ�L�᪺ñ��szSignature, 
�]���̲ת�<ICSRequest>....</ICSRequest>�ʥ]��b szRetPKT ��X
<��J�Ѽ�>
	iICSReqLength : ICSReq�ʥ]����
	szICSReq : ���e�Ҳ��ͪ�ICSReq�ʥ]���e
	iSignatureLength : ñ������
	szSignature : ñ��
	
<��X�Ѽ�>
	iRetPKTLength : �̲שҲ��ͪ�ICSReqest�ʥ]����
	szRetPKT : �̲שҲ��ͪ�ICSReqest�ʥ]���e
<�Ǧ^��>
	 0: ���T�B�z����
	-1: �O����t�m����

ICSRequest�ʥ]�d��
<ICSRequest>
�ʥ]ICSReq�����
<ICSReqLength>�ʥ]ICSReq����ƪ���</ICSReqLength>
<Signature>�ʥ]ICSReq��ñ��</Signature>
</ICSRequest>
*/
extern "C" __declspec(dllexport) int iMake_ICSRequestPKT
(
	int		iICSReqLength,		//ICSReq�ʥ]����
	char	*szICSReq,			//ICSReq�ʥ]
	int		iSignatureLength,	//ICSReq�ʥ]��ñ������
	char	*szSignature,		//ICSReq�ʥ]
	int		*iRetPKTLength,		//�^����ICSRequest�ʥ]����
	char	**szRetPKT			//�^����ICSRequest�ʥ]
);




// Open SSL���ε{�������禡
/*
extern "C" __declspec(dllexport) int iSSL_SendThenRecv
(
	char	*szIP,				//���wServer�ݪ�IP Address
	int		iPort,				//���wServer�ݪ�Port : �@�묰443
	char	*szTrustCACertFN,	//���wñ�oServer���Ҥ�CA (Certificate)
	char	*szCGIURL,			//Server��CGI��������|
	int		iToBeSendData,		//�ݰe��ƪ���
	char	*szToBeSendData,	//�ݰe���
	int		*iRetPKTLength,		//Server�ݦ^�Ǥ���ƪ���
	char	**szRetPKT			//Server�ݦ^�Ǥ����
);
�PServer�إ�SSL�s�u�ñN�ݰe����ƶǦ�Server, �ñ���Server�ݤ��^���ʥ]
<��J�Ѽ�>
	szIP			:���wServer�ݪ�IP Address
	iPort			:���wServer�ݪ�Port : �@�묰443
	szTrustCACertFN	:���wñ�oServer���Ҥ�CA (Certificate)
	szCGIURL		:���wServer��CGI��������|
	iToBeSendData	:�ݰe��ƪ���
	szToBeSendData	:�ݰe���
	
<��X�Ѽ�>
	iRetPKTLength	:Server�ݦ^�Ǥ���ƪ���
	szRetPKT		:Server�ݦ^�Ǥ����

<�Ǧ^��>
	 0: ���T�B�z����
	-1: �O����t�m����
	-2: �_�lWinSock���~
	-3: WinSock����
	-4: �_�lSSL���~
	-5: �t�mSSL�ϥΤ��ܼƥ���
	-6: ���oCA���Ҧ�m����(�Y�ݭn���Ҫ���)
	-7: �t�mSocket����
	-8: �ഫ������}����
	-9: �s�u����
	-10: �t�mSSL�ϥΤ��ܼƥ���
	-11: �]�wSocket��SSL����
	-12: SSL�s�u����
	-13: �o��Server�ݪ����ҥ���
	-14: �^��Server���Ҥ�SubjectName����
	-15: �^��Server���Ҥ�IssuerName����
	-16: �L�k�ǰe�ݰe��Ʀ�Server��
*/
extern "C" __declspec(dllexport) int iSSL_SendThenRecv
(
	char	*szIP,				//���wServer�ݪ�IP Address
	int		iPort,				//���wServer�ݪ�Port : �@�묰443
	char	*szTrustCACertFN,	//���wñ�oServer���Ҥ�CA (Certificate)
	char	*szCGIURL,			//Server��CGI��������|
	int		iToBeSendData,		//�ݰe��ƪ���
	char	*szToBeSendData,	//�ݰe���
	int		*iRetPKTLength,		//Server�ݦ^�Ǥ���ƪ���
	char	**szRetPKT			//Server�ݦ^�Ǥ����
);






/*
extern "C" __declspec(dllexport) int iParse_ICSResponsePKT
(
	ICSDATA		*lpRawICSData,		//�^�s�ѪR�ʥ]�ᤧ�d�絲�G
	int			iResponsePKTLength,	//Server�^�����ʥ]����
	char		*szResponsePKT		//Server�^�����ʥ]
);
�ѪRServre�^�Ǫ�<ICSResponse>....</ICSResponse>�ʥ] ���X�d�絲�G���lpRawICSData->lpSNPID->Code
<��J�Ѽ�>
	iResponsePKTLength : Server�^�Ǫ�ICSResponse�ʥ]����
	szResponsePKT : Server�^�Ǫ�ICSResponse�ʥ]���e
<��X�Ѽ�>
	lpRawICSData : �N�d�絲�G�^�s��
		���槹���禡��
		case 1 �YServer�ݥ��T�B�z()
				--> lpRawICSData->_Result=0; �B�N�d�絲�G�^�s��lpRawICSData->lpSNPID->_Code
		case 2 �YServer�ݥ��ॿ�T�B�z
				--> lpRawICSData->_Result=Server�^�Ǫ����~��, lpRawICSData->lpSNPID->_Code����ѦҷN�q

<�Ǧ^��> 0, -1, 2000, 2001~2017
	0    : ���禡���T�B�z����, �d�絲�G�s��� lpRawICSData->lpSNPID->_Code
			Server�^�����d�ߵ��G�N�X�ѻ��p�U:	
				0: ��ܦ���SerialNumber�w���ӽ�MOICA���ҡA�B�Pclient�ݩҦC�������Ҧr���۲ŦX�A
				   �ӥB��MOICA���Ҥw�g�}�d(���ĩΰ���)�A�o�O�@��ŦX�ݨD�����A
				1: ��ܦ���(SerialNumber+�����Ҧr��)��ƹ�A��Ʈw���S������������
				2: ��ܦ������Ҧr���A�S������������
				3: ��ܦ���SerialNumber�w�ӽ�MOICA���ҡA�B�Pclient�ݩҦC�������Ҧr���۲ŦX�A���O��MOICA���ҩ|���}�d
				//4: ��ܦ���SerialNumber�w�ӽ�MOICA���ҡA�B�Pclient�ݩҦC�������Ҧr���۲ŦX�A���O��MOICA���Ҥw�L���Τw�o��
				4: ��ܦ���SerialNumber�w�ӽ�MOICA���ҡA�B�Pclient�ݩҦC�������Ҧr���۲ŦX�A���O��MOICA���Ҥw�L��
				5: ��ܦ���SerialNumber�w�ӽ�MOICA���ҡA�B�Pclient�ݩҦC�������Ҧr���۲ŦX�A���O��MOICA���Ҥw�o��
				(4,5�i�ण��ڰϤ��X��)
				�p�G�����T�{���G�ҶǦ^����ƭȤ��ݤW�z�d��A�B�j��1000�A�Y�O�t�γB�z�ɵo�Ϳ��~�Ҳ��ͪ����~�N�X�C

	-1   : Server��������, ��WebServer�^�����~�α�����DHttp�ʥ]�榡
	2000 : Server�w�����Ʀ��^�����~�T��, 
			�ӿ��~�T���s���lpRawICSData->_Result, �N�X�p�U:
				ERR_NoCGIParam             1001 //��J��CGI�ѼƬ��ť�,�S����
				ERR_ICSRequest_NotFound    1002 //���X <ICSRequest> �Ѽƥ���
				ERR_ICSReq_NotFound        1003 //���X <ICSReq> �Ѽƥ���
				ERR_ClientID_NotFound      1004 //���X <ClientID> �Ѽƥ���
				ERR_Nonce_NotFound         1005 //���X <Nonce> �Ѽƥ���
				ERR_ReqTime_NotFound       1006 //���X <ReqTime> �Ѽƥ���
				ERR_TotalCount_NotFound    1007 //���X <TotalCount> �Ѽƥ���
				ERR_TotalCount_BadFormat   1008 //�Ѽ� <TotalCount> �榡���~:�D�ƭ�
				ERR_TotalCount_BadValue    1009 //�Ѽ� <TotalCount> �榡���~:�ƭȶW�L�d��
				ERR_ICSReqLength_NotFound  1011 //���X <ICSReqLength> �Ѽƥ���
				ERR_ICSReqLength_BadFormat 1012 //�Ѽ� <ICSReqLength> �榡���~:�D�ƭ�
				ERR_ICSReqLength_NotMatch  1013 //�Ѽ� <ICSReqLength> ���ȩM<ICSReq>�����פ��P
				ERR_Signature_NotFound     1014 //���X <Signature> �Ѽƥ���
				ERR_Signature_BadFormat    1015 //�Ѽ� <Signature> �榡���~,�L�k�qHEX�榡�নbinary��
				ERR_Rec_n_Fail             -10000   //���X�� n �� <Rec> �Ѽƥ���,��TotalCount�Ȥ����T
				ERR_ID_n_Fail              -20000  //���X�� n �� <ID> �Ѽƥ���,�θ�ƿ��~,�Τ��O�y����
				ERR_SN_n_Fail              -30000   //���X�� n �� <SN> �Ѽƥ���,�θ�ƿ��~
				ERR_SSID_n_Fail            -40000   //���X�� n �� <SSID>�Ѽƥ���,�θ�ƿ��~
				ERR_ClientID_Incorrect     1020 //�t�Τ��{�i��ClientID,�t�εL��ClientID����T
				ERR_Verify_Signature_Fail  1021 //����client�ݩҰ���ñ������
				ERR_ConnectDB_Fail         1022 //�s�u���Ʈw����

	2001 ~ 2017 : Client ����^����Ʀ��ѪR�ʥ]�ɥX��, ���~�N�X�p�U:
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
	ICSDATA		*lpRawICSData,		//�^�s�ѪR�ʥ]�ᤧ�d�絲�G
	int			iResponsePKTLength,	//Server�^�����ʥ]����
	char		*szResponsePKT		//Server�^�����ʥ]
);

// ����Free��Function 
extern "C" __declspec(dllexport) void vFreeData (char *szData);

//�NBinary�নhex �N�G�i��ñ�����ন16�i��ɥΨ�
extern "C" __declspec(dllexport) int bin2hex(unsigned char *input, int length, char *output);

