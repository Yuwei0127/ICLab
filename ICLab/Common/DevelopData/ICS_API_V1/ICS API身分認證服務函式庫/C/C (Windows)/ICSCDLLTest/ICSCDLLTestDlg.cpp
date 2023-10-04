// ICSCDLLTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ICSCDLLTest.h"
#include "ICSCDLLTestDlg.h"
#include <iostream>


#include "ICSCDLL.h"

//HiSecure
#include "hisecure/chthisecurecryptoapiva.h"
#include "hisecure/chthisecurestructva.h"
#include "hisecure/GPKICFunction.h"
#include "hisecure/pkcs11.h"
#include "hisecure/errortable.h"

int Sign_with_HiSecure
(
	char	*szPrivateKeyFN,	//�K�_�ɦW
	char	*szPassWord,		//�Ѷ}�K�_�ɩһݤ��K�X
	int		iPassWordLength,	//�K�X����
	char	*szToBeSignedData,	//��ñ���T��
	int		iToBeSignedData,	//��ñ���T������
	char	**szSignatureHEX,	//ñ�����G(HEX�榡)
	int		*iSignatureHEX		//ñ������
);

int iIDCertCheck_HiSecure
(
	ICSDATA	*lpICSData,				//�����Ҹ�, ���ҧǸ�...����l���
									//�άd�絲�G�����ʥ](�Ԩ�ICSCLib.h)
	int		iSeed,					//���ͫʥ]Nonce�ҭn���üƺؤl
	char	*szIP,					//���wServer�ݪ�IP Address
	int		iPort,					//���wServer�ݪ�Port : �@�묰443
	char	*szClientCertFN,		//v3 ���wClient�ݪ�������
	char	*szTrustCACertFN,		//���wñ�oServer���Ҥ�CA (Certificate)
	char	*szPrivateKeyFN,		//�p�K���_�ɤ��ɦW
	char	*szPassWord,			//�Ѷ}�p�K���_�ɤ��K�X
	int		iPassWord,				//�K�X����
	char	*szCGIURL,				//Server��CGI��������|
	int		*iRequestPKTLength,		//���禡�]����Request�ʥ]����
	char	**szRequestPKT,			//���禡�]����Request�ʥ]
	int		*iResponsePKTLength,	//���禡���쪺Response�ʥ]����
	char	**szResponsePKT,		//���禡���쪺Response�ʥ]
	int		*iErrorCode				//�Y�����o�Ϳ��~, �|�]�wErrorCode
);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICSCDLLTestDlg dialog

CICSCDLLTestDlg::CICSCDLLTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CICSCDLLTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CICSCDLLTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CICSCDLLTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CICSCDLLTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CICSCDLLTestDlg, CDialog)
	//{{AFX_MSG_MAP(CICSCDLLTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ICSCDLLTest, OnICSCDLLTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICSCDLLTestDlg message handlers

BOOL CICSCDLLTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CICSCDLLTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CICSCDLLTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CICSCDLLTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///             ID Certificate Check with HiSecure                    ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int Sign_with_HiSecure
(
	char	*szPrivateKeyFN,	//�K�_�ɦW
	char	*szPassWord,		//�Ѷ}�K�_�ɩһݤ��K�X
	int		iPassWordLength,	//�K�X����
	char	*szToBeSignedData,	//��ñ���T��
	int		iToBeSignedData,	//��ñ���T������
	char	**szSignatureHEX,	//ñ�����G(HEX�榡)
	int		*iSignatureHEX		//ñ������
)
{
	int iRet;
	unsigned long ulModule, ulSession;
	PrivateKeyInputData PvkeyIDStruct; 

	FILE *fPVK;
	int iPVKLength;
	unsigned char *szPVKData;
	unsigned long ulPrivateKey;

	unsigned char	*szSignature=NULL;
	int				iSignature;
	char			*szTmp=NULL;
	int				iTmp=0;

	iTmp=strlen(szPrivateKeyFN);
	//Check ���ɦW�O�_��.pfx�榡���ɮ�
	if (   ((szPrivateKeyFN[iTmp-3]=='P')||(szPrivateKeyFN[iTmp-3]=='p'))
		&& ((szPrivateKeyFN[iTmp-2]=='F')||(szPrivateKeyFN[iTmp-2]=='f'))
		&& ((szPrivateKeyFN[iTmp-1]=='X')||(szPrivateKeyFN[iTmp-1]=='x')) )
	{
		//Ū�J PKCS12 (.pfx�榡) �p�K���_
		if ( (fPVK=fopen(szPrivateKeyFN, "rb")) == NULL) return -3; //Private Key File Open Error
		else
		{
			fseek(fPVK, 0, SEEK_END);
			iPVKLength=ftell(fPVK);
			fseek(fPVK, 0, SEEK_SET);
			szPVKData = (unsigned char*)malloc(iPVKLength);
			if (szPVKData==NULL)
			{
				fclose(fPVK);
				return -4;
			}
			iRet = fread( szPVKData, sizeof(char), iPVKLength,fPVK);
			fclose(fPVK);
		}

		//PKCS12 .pfx�ɮ׮榡, ������ chtpkcs12dll.dll 
		iRet = InitModule("chtpkcs12dll.dll", szPVKData, &ulModule);
		if (iRet != 0) return -1; //Initial Module error

		// ��l�禡
		iRet = InitSession(ulModule, CKF_SERIAL_SESSION, szPassWord, iPassWordLength, &ulSession);
		if (iRet != 0) return -2; //Initial Session error

		free(szPVKData); szPVKData=NULL;
		iRet = GetKeyObjectHandle(ulModule, ulSession, 0, NULL, 0, "1", 1, &ulPrivateKey);
		if (iRet!=0) return -6;
	}
	else
	{
		//	�_�l PKCS8 �K�X�Ҳ�(Module)�P�禡(Session)
		//	�����l�U�ӼҲժ��_�l�禡�ϥΤ覡�i�H�ѷӨ�ާ@����
		//	���J��dll�ɦW�ٶ����T������K�X�Ҳ�
		iRet = InitModule("chtbasicdll.dll", NULL, &ulModule);

		if (iRet != 0) return -1; //Initial Module error

		// ��l�禡
		iRet = InitSession(ulModule, CKF_SERIAL_SESSION, NULL, 0, &ulSession);

		if (iRet != 0) return -2; //Initial Session error

		//Ū�J PKCS8 �p�K���_
		if ( (fPVK=fopen(szPrivateKeyFN, "rb")) == NULL) return -3; //Private Key File Open Error
		else
		{
			fseek(fPVK, 0, SEEK_END);
			iPVKLength=ftell(fPVK);
			fseek(fPVK, 0, SEEK_SET);
			szPVKData = (unsigned char*)malloc(iPVKLength);
			if (szPVKData==NULL)
			{
				fclose(fPVK);
				return -4;
			}
			iRet = fread( szPVKData, sizeof(char), iPVKLength,fPVK);
			fclose(fPVK);
		}

		//���o�p�K���_ Handle
		iRet=GenerateGCAv3PrivateKeyStruct(szPVKData, iPVKLength, szPassWord, iPassWordLength, PvkeyIDStruct);
		free(szPVKData); szPVKData=NULL;
		if (iRet!=0) return -5;
		iRet = GetKeyObjectHandle(ulModule, ulSession, 0, NULL, 0, (void*)&PvkeyIDStruct, 1, &ulPrivateKey);
		if (iRet!=0) return -6;
	}
	
	
	//�i��ñ��
	szSignature=NULL;
	iSignature=1024; // ���_���׬� 1024bits �ɡAñ�����׬� 128bytes
	szSignature=(unsigned char*)malloc(iSignature);
	if (szSignature==NULL) return -7; //memory allocation error

	iRet=MakeSignature
		 (
			ulModule,
			ulSession,
			CKM_SHA1_RSA_PKCS,
			(unsigned char*) szToBeSignedData,
			(const int) iToBeSignedData,
			ulPrivateKey,
			szSignature,
			&iSignature
		 );
	if (iRet!=0)
	{
		//���_���A�ϥΡA�I�sDeleteKeyObject�禡����ҥe�Τ��O����	
		DeleteKeyObject(ulModule, ulSession, ulPrivateKey);
		//�����K�X�Ҳ�(Module)�P�禡(Session)
		CloseSession ( ulModule, ulSession );
		CloseModule ( ulModule );
		//���^szSignature���O����
		free(szSignature); szSignature=NULL;
		return -8;
	}
	

	//�ഫ��HEX�榡
	szTmp=(char *) malloc(iSignature*2+1);
	if (szTmp==NULL)
	{
		//���_���A�ϥΡA�I�sDeleteKeyObject�禡����ҥe�Τ��O����	
		DeleteKeyObject(ulModule, ulSession, ulPrivateKey);
		//�����K�X�Ҳ�(Module)�P�禡(Session)
		CloseSession ( ulModule, ulSession );
		CloseModule ( ulModule );
		//���^szSignature���O����
		free(szSignature); szSignature=NULL;
		return -9;
	}

	bin2hex(szSignature, iSignature, szTmp);
	*szSignatureHEX=szTmp;
	*iSignatureHEX=iSignature*2;

	//���_���A�ϥΡA�I�sDeleteKeyObject�禡����ҥe�Τ��O����	
	DeleteKeyObject(ulModule, ulSession, ulPrivateKey);
	//�����K�X�Ҳ�(Module)�P�禡(Session)
	CloseSession ( ulModule, ulSession );
	CloseModule ( ulModule );
	//���^szSignature���O����
	free(szSignature); szSignature=NULL;

	return 0;
}

int iIDCertCheck_HiSecure
(
	ICSDATA	*lpICSData,				//�����Ҹ�, ���ҧǸ�...����l���
									//�άd�絲�G�����ʥ](�Ԩ�ICSCLib.h)
	int		iSeed,					//���ͫʥ]Nonce�ҭn���üƺؤl
	char	*szIP,					//���wServer�ݪ�IP Address
	int		iPort,					//���wServer�ݪ�Port : �@�묰443
	char	*szClientCertFN,		//v3 ���wClient�ݪ�������
	char	*szTrustCACertFN,		//���wñ�oServer���Ҥ�CA (Certificate)
	char	*szPrivateKeyFN,		//�p�K���_�ɤ��ɦW
	char	*szPassWord,			//�Ѷ}�p�K���_�ɤ��K�X
	int		iPassWord,				//�K�X����
	char	*szCGIURL,				//Server��CGI��������|
	int		*iRequestPKTLength,		//���禡�]����Request�ʥ]����
	char	**szRequestPKT,			//���禡�]����Request�ʥ]
	int		*iResponsePKTLength,	//���禡���쪺Response�ʥ]����
	char	**szResponsePKT,		//���禡���쪺Response�ʥ]
	int		*iErrorCode				//�Y�����o�Ϳ��~, �|�]�wErrorCode
)
{
	int		iRet;
	int		iSignatureHEX=0, iICSReq=0, iICSRequest=0, iICSResponse=0;
	char	*szSignatureHEX=NULL, *szICSReq=NULL, *szICSRequest=NULL, *szICSResponse=NULL;
	char	szOut[50000]; //2020/12/01�s�W���ըϥ�

	*iRequestPKTLength=0;
	*szRequestPKT=NULL;
	*iResponsePKTLength=0;
	*szResponsePKT=NULL;
	*iErrorCode=0;
	
	//v3
	iRet=iMake_ICSReqPKT( iSeed, szClientCertFN, lpICSData , &iICSReq, &szICSReq);
	if (iRet!=0) return -810; //Memory Allocation Failure : szICSReq


	iRet=Sign_with_HiSecure
		 (
			szPrivateKeyFN,
			szPassWord,
			iPassWord,
			szICSReq,
			iICSReq,
			&szSignatureHEX,
			&iSignatureHEX
		 );
	if(iRet!=0)
	{
		//pem key sign error, error code: iRet
		vFreeData(szICSReq);
		*iErrorCode=iRet;
		return -820;
	}

	iRet=iMake_ICSRequestPKT(iICSReq, szICSReq, iSignatureHEX, szSignatureHEX, &iICSRequest, &szICSRequest);
	if (iRet!=0) 
	{
		//Memory Allocation Failure : szICSRequestPKT
		vFreeData(szICSReq);
		free(szSignatureHEX); szSignatureHEX=NULL;
		return -830;
	}

	vFreeData(szICSReq);
	free(szSignatureHEX); szSignatureHEX=NULL;

	*iRequestPKTLength=iICSRequest;
	*szRequestPKT=szICSRequest;

	//*************2020/12/01�s�W���ըϥ�**************
	//sprintf(szOut, "�d�߫ʥ]����:\n\n\n\n%d", iICSRequest);
	//AfxMessageBox(szOut);
	sprintf(szOut, "�d�߫ʥ](Request Packet)���e�p�U:\n\n\n\n%s", szICSRequest);
	AfxMessageBox(szOut);
	//*************************************************

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

	//*************2020/12/01�s�W���ըϥ�**************
	//sprintf(szOut, "�^���ʥ]����:\n\n\n\n%d", iICSResponse);
	//AfxMessageBox(szOut);
	sprintf(szOut, "�^���ʥ](Response Packet)���e�p�U:\n\n\n\n%s", szICSResponse);
	AfxMessageBox(szOut);
	//*************************************************

	iRet = iParse_ICSResponsePKT( lpICSData, iICSResponse, szICSResponse);
	if (iRet!=0)
	{
		*iErrorCode=iRet;
		return -850;
	}

	return 0;
}




void CICSCDLLTestDlg::OnICSCDLLTest() 
{
	int		iRet, iErrorCode, i;
	ICSDATA	*lpICSData=NULL;
	int		iRequest=0, iResponse=0;
	char	*szRequest=NULL, *szResponse=NULL;
	FILE	*StatusFile;
	char	szDataBuf[256], szIP[256], szTrustCACert[256], szCGIURL[256], szKeyFile[256], szPassWord[256], szClientCertFile[256]; //v3
	int		iPort;
	bool	iChoiceHiSecure;
	char	szTmp[230], szOut[50000];


	StatusFile = fopen( "ICSCTest.txt", "r+t" );
	if( StatusFile == NULL )
	{
		iChoiceHiSecure=true;
		strcpy(szIP, "10.144.133.180");
		iPort=443;
		strcpy(szTrustCACert,"GTestCA.pem");
		strcpy(szCGIURL, "/cgi-bin/CheckSNPID/CheckSNPID");
		strcpy(szClientCertFile,"cert12345.cer"); //v3
		strcpy(szKeyFile, "Test1.p8b");
		strcpy(szPassWord, "12345678");
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
		fscanf( StatusFile, "%s", szDataBuf);
		fscanf( StatusFile, "%s", szDataBuf);
		strcpy(szPassWord, szDataBuf);

		fclose(StatusFile);
	}


	//Ready Data
	lpICSData=(ICSDATA *) malloc(sizeof(ICSDATA));
	if (lpICSData==NULL ) { } // Memory allocate failure 
	lpICSData->Count=7;
	lpICSData->lpSNPID = (SNPID *)malloc(lpICSData->Count*sizeof(SNPID));
	if (lpICSData->lpSNPID==NULL ) { } // Memory allocate failure

	//v3 strcpy(lpICSData->_ClientID, szID);
	//strcpy( (lpICSData->lpSNPID+0)->SN,  "0000000115948071" );
	//strcpy( (lpICSData->lpSNPID+0)->PID, "A123456789");
	//strcpy( (lpICSData->lpSNPID+0)->PID, "J222563561");
	/*
	strcpy( (lpICSData->lpSNPID+1)->SN,  "0000000010184986" );
	strcpy( (lpICSData->lpSNPID+1)->PID, "L423154314");
	strcpy( (lpICSData->lpSNPID+2)->SN,  "0000000115959317" );
	strcpy( (lpICSData->lpSNPID+2)->PID, "G121353503");
	strcpy( (lpICSData->lpSNPID+3)->SN,  "0000000010000774" );
	strcpy( (lpICSData->lpSNPID+3)->PID, "E200213219");
	strcpy( (lpICSData->lpSNPID+4)->SN,  "0000000010184987" );
	strcpy( (lpICSData->lpSNPID+4)->PID, "B321535173");
	strcpy( (lpICSData->lpSNPID+5)->SN,  "0000000115931790" );
	strcpy( (lpICSData->lpSNPID+5)->PID, "T123650201");
	strcpy( (lpICSData->lpSNPID+6)->SN,  "0000000115959183" );
	strcpy( (lpICSData->lpSNPID+6)->PID, "M221910120");
	strcpy( (lpICSData->lpSNPID+7)->SN,  "0000000010010863" );
	strcpy( (lpICSData->lpSNPID+7)->PID, "A533456789");
	strcpy( (lpICSData->lpSNPID+8)->SN,  "0000000010184904" );
	strcpy( (lpICSData->lpSNPID+8)->PID, "O423456799");
	*/
	/*
	strcpy( (lpICSData->lpSNPID+9)->SN,  "0000000011415254" );
	strcpy( (lpICSData->lpSNPID+9)->PID, "Y123456784");
	*/
	/*
	strcpy( (lpICSData->lpSNPID+10)->SN,  "0000000115948071" );
	strcpy( (lpICSData->lpSNPID+10)->PID, "J222563561");
	strcpy( (lpICSData->lpSNPID+11)->SN,  "0000000010184986" );
	strcpy( (lpICSData->lpSNPID+11)->PID, "L423154314");
	strcpy( (lpICSData->lpSNPID+12)->SN,  "0000000115959317" );
	strcpy( (lpICSData->lpSNPID+12)->PID, "G121353503");
	strcpy( (lpICSData->lpSNPID+13)->SN,  "0000000010000774" );
	strcpy( (lpICSData->lpSNPID+13)->PID, "E200213219");
	strcpy( (lpICSData->lpSNPID+14)->SN,  "0000000010184987" );
	strcpy( (lpICSData->lpSNPID+14)->PID, "B321535173");
	strcpy( (lpICSData->lpSNPID+15)->SN,  "0000000115931790" );
	strcpy( (lpICSData->lpSNPID+15)->PID, "T123650201");
	strcpy( (lpICSData->lpSNPID+16)->SN,  "0000000115959183" );
	strcpy( (lpICSData->lpSNPID+16)->PID, "M221910120");
	strcpy( (lpICSData->lpSNPID+17)->SN,  "0000000010010863" );
	strcpy( (lpICSData->lpSNPID+17)->PID, "A533456789");
	strcpy( (lpICSData->lpSNPID+18)->SN,  "0000000010184904" );
	strcpy( (lpICSData->lpSNPID+18)->PID, "O423456799");
	strcpy( (lpICSData->lpSNPID+19)->SN,  "0000000011415254" );
	strcpy( (lpICSData->lpSNPID+19)->PID, "Y123456784");
	*/
    /*
	strcpy( (lpICSData->lpSNPID+20)->SN,  "0000000115948071" );
	strcpy( (lpICSData->lpSNPID+20)->PID, "J222563561");
	strcpy( (lpICSData->lpSNPID+21)->SN,  "0000000010184986" );
	strcpy( (lpICSData->lpSNPID+21)->PID, "L423154314");
	strcpy( (lpICSData->lpSNPID+22)->SN,  "0000000115959317" );
	strcpy( (lpICSData->lpSNPID+22)->PID, "G121353503");
	strcpy( (lpICSData->lpSNPID+23)->SN,  "0000000010000774" );
	strcpy( (lpICSData->lpSNPID+23)->PID, "E200213219");
	strcpy( (lpICSData->lpSNPID+24)->SN,  "0000000010184987" );
	strcpy( (lpICSData->lpSNPID+24)->PID, "B321535173");
	strcpy( (lpICSData->lpSNPID+25)->SN,  "0000000115931790" );
	strcpy( (lpICSData->lpSNPID+25)->PID, "T123650201");
	strcpy( (lpICSData->lpSNPID+26)->SN,  "0000000115959183" );
	strcpy( (lpICSData->lpSNPID+26)->PID, "M221910120");
	strcpy( (lpICSData->lpSNPID+27)->SN,  "0000000010010863" );
	strcpy( (lpICSData->lpSNPID+27)->PID, "A533456789");
	strcpy( (lpICSData->lpSNPID+28)->SN,  "0000000010184904" );
	strcpy( (lpICSData->lpSNPID+28)->PID, "O423456799");
	strcpy( (lpICSData->lpSNPID+29)->SN,  "0000000011415254" );
	strcpy( (lpICSData->lpSNPID+29)->PID, "Y123456784");
	*/
	/*
	strcpy( (lpICSData->lpSNPID+10)->SN,  "0000000115945601" );
	strcpy( (lpICSData->lpSNPID+10)->PID, "Q121245503<Birth>19650422</Birth><Phone>_</Phone><Cemail>_</Cemail><ACode>64000150</ACode>");
	strcpy( (lpICSData->lpSNPID+11)->SN,  "0000000115948071" );
	strcpy( (lpICSData->lpSNPID+11)->PID, "J222563561<Birth>19900712</Birth><Phone>_</Phone><Cemail>_</Cemail><ACode>65000110</ACode>");
	*/
	///*�����諸���
	strcpy( (lpICSData->lpSNPID+0)->SN,  "0000000115948071" );
	strcpy( (lpICSData->lpSNPID+0)->PID, "J222563561");
	strcpy( (lpICSData->lpSNPID+1)->SN,  "0000000115959317" );
	strcpy( (lpICSData->lpSNPID+1)->PID, "G121353503");
	strcpy( (lpICSData->lpSNPID+2)->SN,  "0000000115931790" );
	strcpy( (lpICSData->lpSNPID+2)->PID, "T123650201");
	strcpy( (lpICSData->lpSNPID+3)->SN,  "0000000115959183" );
	strcpy( (lpICSData->lpSNPID+3)->PID, "M221910120");
	strcpy( (lpICSData->lpSNPID+4)->SN,  "0000000111900263" );
	strcpy( (lpICSData->lpSNPID+4)->PID, "Q121482042");
	strcpy( (lpICSData->lpSNPID+5)->SN,  "0000000115959317" );
	strcpy( (lpICSData->lpSNPID+5)->PID, "G121353503");
	strcpy( (lpICSData->lpSNPID+6)->SN,  "0000000113023163" );
	strcpy( (lpICSData->lpSNPID+6)->PID, "A121583078");
    //*/


	if (iChoiceHiSecure)
	{
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
	}
	else //OpenSSLñ������
	{
		AfxMessageBox("���䴩�ϥ�OpenSSLñ���\��");
		iRet=-999;
		/*
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
		*/
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
		strcat(szOut,"1023 ����client�ݾ��Ҫ��Ĵ�����\n");

		if (iRet!=-999) AfxMessageBox(szOut);
		//put your error processing code here...

	}
	else
	{
		//sucess
		//sprintf(szOut, "�d�߫ʥ](Request Packet)���e�p�U:\n\n\n\n%s", szRequest);
		//AfxMessageBox(szOut);
		//sprintf(szOut, "�^���ʥ](Response Packet)���e�p�U:\n\n\n\n%s", szResponse);
		//AfxMessageBox(szOut);

		sprintf(szOut, "�ѪR���G�p�U:\n\n\n\nClient ID:%s\nNonce:%s\nDateTime:%s\nResult:%d\nCount:%d\n",
						lpICSData->_ClientID, //v3
						lpICSData->_Nonce, 
						lpICSData->_DateTime,
						lpICSData->_Result,
						lpICSData->Count
				);
		//for (i=1;i<=9;i++)
		for (i=1;i<=lpICSData->Count;i++)
		{
			sprintf(szTmp,"SN:%s     PID:%s     Code:%d\n", 
							(lpICSData->lpSNPID+i-1)->SN,
							(lpICSData->lpSNPID+i-1)->PID,
							(lpICSData->lpSNPID+i-1)->_Code );
			strcat(szOut,szTmp);
		}
		AfxMessageBox(szOut);	
	}


	//���שI�s�O�_���\ �Ҷ��b�B�z��������Ҧ��Ϊ��O����
	vFreeData(szRequest);
	vFreeData(szResponse);
	if (lpICSData->lpSNPID!=NULL) { free(lpICSData->lpSNPID); lpICSData->lpSNPID=NULL; }
	if (lpICSData!=NULL) { free(lpICSData); lpICSData=NULL; }

}
