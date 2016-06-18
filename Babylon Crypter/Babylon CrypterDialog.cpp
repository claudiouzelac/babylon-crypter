
// Babylon CrypterDialog.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "Babylon Crypter.h"
#include "Babylon CrypterDialog.h"
#include "afxdialogex.h"

#include "SDK\SDK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CBabylonCrypterDialog::CBabylonCrypterDialog(CWnd* pParent) : CDialogEx(CBabylonCrypterDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBabylonCrypterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBabylonCrypterDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOADFILE_BUTTON, &CBabylonCrypterDialog::OnBnClickedLoadfileButton)
	ON_BN_CLICKED(IDC_LOADSTUB_BUTTON, &CBabylonCrypterDialog::OnBnClickedLoadstubButton)
	ON_BN_CLICKED(IDC_CRYPT_BUTTON, &CBabylonCrypterDialog::OnBnClickedCryptButton)
	ON_BN_CLICKED(IDC_LOADICON_BUTTON, &CBabylonCrypterDialog::OnBnClickedLoadiconButton)
END_MESSAGE_MAP()

BOOL CBabylonCrypterDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		
	return TRUE; 
}

void CBabylonCrypterDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CBabylonCrypterDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBabylonCrypterDialog::OnBnClickedLoadfileButton()
{
	OPENFILENAME ofn;
    ZeroMemory(& ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL ;
    ofn.lpstrFile = m_File_Path;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(m_File_Path);
    ofn.lpstrFilter = "Executables\0*.EXE\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
    if(!GetOpenFileName(& ofn))
    {
		return;
    }
	CWnd* pWnd = GetDlgItem(IDC_FILE_EDIT);
	pWnd->SetWindowTextA(m_File_Path);
	
}


void CBabylonCrypterDialog::OnBnClickedLoadstubButton()
{
	OPENFILENAME ofn;
    ZeroMemory(& ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = m_Stub_Path;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(m_Stub_Path);
    ofn.lpstrFilter = "Executables\0*.EXE\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
    if(!GetOpenFileName(& ofn))
    {
		return;
    }
	CWnd* pWnd = GetDlgItem(IDC_STUB_EDIT);
	pWnd->SetWindowTextA(m_Stub_Path);
}


void CBabylonCrypterDialog::OnBnClickedCryptButton()
{

	OPENFILENAME ofn;
    ZeroMemory(& ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
	ofn.lpstrFile = m_Save_Path;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(m_Save_Path);
    ofn.lpstrFilter = "Executables\0*.EXE\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
	if(!GetSaveFileName(& ofn))
    {
		return;
    }


	worker = new BuildWorker(m_Save_Path, m_Stub_Path, m_File_Path);
	worker->xEncryptFile();

	if(m_Icon_Path != NULL)
	{
		worker->ChangeIcon(m_Icon_Path);
	}

	MessageBox("Crypted !", "Babylon Crypt", 0);

}


void CBabylonCrypterDialog::OnBnClickedLoadiconButton()
{
	OPENFILENAME ofn;
    ZeroMemory(& ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
	ofn.lpstrFile = m_Icon_Path;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(m_Icon_Path);
    ofn.lpstrFilter = "Icons\0*.ico\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
    if(!GetOpenFileName(& ofn))
    {
		return;
    }
	CWnd* pWnd = GetDlgItem(IDC_ICON_EDIT);
	pWnd->SetWindowTextA(m_Icon_Path);



}
