
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
};
