
// libGBT28181GUIClientTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "libGBT28181GUIClientTest.h"
#include "libGBT28181GUIClientTestDlg.h"
#include "afxdialogex.h"

#include <libGBT28181Client.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClibGBT28181GUIClientTestDlg dialog



ClibGBT28181GUIClientTestDlg::ClibGBT28181GUIClientTestDlg(CWnd* pParent /*=NULL*/)
    :CDialogEx(ClibGBT28181GUIClientTestDlg::IDD, pParent),
    m_local_client_SIP_user_name(_T("")),
    m_local_client_SIP_user_ID(_T("")),
    m_local_client_SIP_password(_T("")),
    m_local_client_IP(_T("")),
    m_local_client_port(0),
    m_SIP_server_ID(_T("")),
    m_SIP_server_domain(_T("")),
    m_SIP_server_IP(_T("")),
    m_SIP_server_port(0),
    m_register_expiration_interval(0),
    m_heartbeat_interval(0),
    m_maximun_time_out_interval(0),
    m_target_SIP_user_name(_T("")),
    m_target_IP(_T("")),
    m_target_port(0),
    m_info_output(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClibGBT28181GUIClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_LOCAL_CLIENT_SIP_USER_NAME, m_local_client_SIP_user_name);
    DDX_Text(pDX, IDC_EDIT_LOCAL_CLIENT_SIP_USER_ID, m_local_client_SIP_user_ID);
    DDX_Text(pDX, IDC_EDIT_LOCAL_CLIENT_SIP_PASSWORD, m_local_client_SIP_password);
    DDX_Text(pDX, IDC_EDIT_LOCAL_CLIENT_IP, m_local_client_IP);
    DDX_Text(pDX, IDC_EDIT_LOCAL_CLIENT_PORT, m_local_client_port);
    DDX_Text(pDX, IDC_EDIT_SERVER_ID, m_SIP_server_ID);
    DDX_Text(pDX, IDC_EDIT_SERVER_DOMAIN, m_SIP_server_domain);
    DDX_Text(pDX, IDC_EDIT_SERVER_IP, m_SIP_server_IP);
    DDX_Text(pDX, IDC_EDIT_SERVER_PORT, m_SIP_server_port);
    DDX_Text(pDX, IDC_EDIT_REGISTER_EXPIRATION_INTERVAL, m_register_expiration_interval);
    DDX_Text(pDX, IDC_EDIT_HEARTBEAT_INTERVAL, m_heartbeat_interval);
    DDX_Text(pDX, IDC_EDIT_MAXIMUN_TIME_OUT_INTERVAL, m_maximun_time_out_interval);
    DDX_Text(pDX, IDC_EDIT_TARGET_SIP_USER_NAME, m_target_SIP_user_name);
    DDX_Text(pDX, IDC_EDIT_TARGET_IP, m_target_IP);
    DDX_Text(pDX, IDC_EDIT_TARGET_PORT, m_target_port);
    DDX_Text(pDX, IDC_EDIT_INFO_OUTPUT, m_info_output);
}

BEGIN_MESSAGE_MAP(ClibGBT28181GUIClientTestDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// ClibGBT28181GUIClientTestDlg message handlers

BOOL ClibGBT28181GUIClientTestDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetWindowLong(this->m_hWnd, GWL_STYLE, GetWindowLong(this->m_hWnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_SYSMENU);
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    m_local_client_SIP_user_name = _T("34020000001320000029");
    m_local_client_SIP_user_ID = _T("34020000001320000029");
    m_local_client_SIP_password = _T("123456");
    m_local_client_IP = _T("192.168.10.29");
    m_local_client_port = 5060;
    m_SIP_server_ID = _T("34020000002000000001");
    m_SIP_server_domain = _T("3402000000");
    m_SIP_server_IP = _T("192.168.10.67");
    m_SIP_server_port = 5060;
    m_register_expiration_interval = 3600;
    m_heartbeat_interval = 60;
    m_maximun_time_out_interval = 3;
    m_info_output = _T("All kinds of info out put area.\r\n");
    UpdateData(FALSE);

    int32_t result = GBT28181_client_initial();
    TCHAR message[256];
    _sntprintf(message, 256, _T("GBT28181_client_initial return: %d\r\n"), result);
    m_info_output += message;
    UpdateData(FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ClibGBT28181GUIClientTestDlg::OnPaint()
{
    if(IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ClibGBT28181GUIClientTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}



BOOL ClibGBT28181GUIClientTestDlg::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    uint32_t result = GBT28181_free_client();
    TCHAR message[256];
    if(GBT28181_SUCCESS != result)
    {
        _sntprintf(message, 256, _T("GBT28181_free_client error: %d"), result);
        AfxMessageBox(message);
    }
    return CDialogEx::DestroyWindow();
}
