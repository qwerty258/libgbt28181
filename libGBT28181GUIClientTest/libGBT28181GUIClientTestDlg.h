
// libGBT28181GUIClientTestDlg.h : header file
//

#pragma once


// ClibGBT28181GUIClientTestDlg dialog
class ClibGBT28181GUIClientTestDlg : public CDialogEx
{
    // Construction
public:
    ClibGBT28181GUIClientTestDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum
    {
        IDD = IDD_LIBGBT28181GUICLIENTTEST_DIALOG
    };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
private:
    CString m_local_client_SIP_user_name;
    CString m_local_client_SIP_user_ID;
    CString m_local_client_SIP_password;
    CString m_local_client_IP;
    DWORD m_local_client_port;
    CString m_SIP_server_ID;
    CString m_SIP_server_domain;
    CString m_SIP_server_IP;
    DWORD m_SIP_server_port;
    DWORD m_register_expiration_interval;
    DWORD m_heartbeat_interval;
    DWORD m_maximun_time_out_number;
    CString m_target_SIP_user_name;
    CString m_target_IP;
    DWORD m_target_port;
    int m_min_pos;
    int m_max_pos;
public:
    CString m_info_output;

    virtual BOOL DestroyWindow();
    char* CstringToChar(CString& cstring);
    int SetFunctionWithCharParameter(int(*p_function)(char*), CString& cstringToSet);
    afx_msg void OnClickedButtonGoOnline();
    afx_msg void OnClickedButtonQueryDeviceStatus();
    afx_msg void OnClickedButtonQueryDeviceInfo();
    afx_msg void OnClickedButtonQueryCatalog();
    afx_msg void OnClickedButtonUpdateInfo();
};
