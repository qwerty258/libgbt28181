
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
    int m_play_instance;
    DWORD m_live_time_stream_handle;
    BYTE m_tilt_speed;
    BYTE m_pan_speed;
    BYTE m_zoom_speed;
public:
    CString m_info_output;

    virtual BOOL DestroyWindow();
    int SetFunctionWithCharParameter(int(*p_function)(char*), CString& cstringToSet);
    afx_msg void OnClickedButtonGoOnline();
    afx_msg void OnClickedButtonQueryDeviceStatus();
    afx_msg void OnClickedButtonQueryDeviceInfo();
    afx_msg void OnClickedButtonQueryCatalog();
    afx_msg void OnClickedButtonUpdateInfo();
    afx_msg void OnClickedButtonGetLiveVideo();
    afx_msg void OnClickedButtonCloseVideo();
    afx_msg void OnClickedButtonDown();
    afx_msg void OnClickedButtonLeft();
    afx_msg void OnClickedButtonLeftDown();
    afx_msg void OnClickedButtonLeftUp();
    afx_msg void OnClickedButtonRight();
    afx_msg void OnClickedButtonRightDown();
    afx_msg void OnClickedButtonRightUp();
    afx_msg void OnClickedButtonUp();
    afx_msg void OnClickedButtonZoomIn();
    afx_msg void OnClickedButtonZoomOut();
};
