
// libGBT28181GUIClientTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "libGBT28181GUIClientTest.h"
#include "libGBT28181GUIClientTestDlg.h"
#include "afxdialogex.h"
#include "CStringToChar.h"

#include <libGBT28181Client.h>
#include <PlayH264DLL.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PTZ_SLEEP 1000

ClibGBT28181GUIClientTestDlg* p_global_ClibGBT28181GUIClientTestDlg;

// ClibGBT28181GUIClientTestDlg dialog

int global_instance;

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
    m_maximun_time_out_number(0),
    m_target_SIP_user_name(_T("")),
    m_target_IP(_T("")),
    m_target_port(0),
    m_info_output(_T("")),
    m_tilt_speed(0),
    m_pan_speed(0),
    m_zoom_speed(0)
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
    DDX_Text(pDX, IDC_EDIT_MAXIMUN_TIME_OUT_INTERVAL, m_maximun_time_out_number);
    DDX_Text(pDX, IDC_EDIT_TARGET_SIP_USER_NAME, m_target_SIP_user_name);
    DDX_Text(pDX, IDC_EDIT_TARGET_IP, m_target_IP);
    DDX_Text(pDX, IDC_EDIT_TARGET_PORT, m_target_port);
    DDX_Text(pDX, IDC_EDIT_INFO_OUTPUT, m_info_output);
    DDX_Text(pDX, IDC_EDIT_TILT_SPEED, m_tilt_speed);
    DDV_MinMaxByte(pDX, m_tilt_speed, 0, 255);
    DDX_Text(pDX, IDC_EDIT_PAN_SPEED, m_pan_speed);
    DDV_MinMaxByte(pDX, m_pan_speed, 0, 255);
    DDX_Text(pDX, IDC_EDIT_ZOOM_SPEED, m_zoom_speed);
    DDV_MinMaxByte(pDX, m_zoom_speed, 0, 16);
}

BEGIN_MESSAGE_MAP(ClibGBT28181GUIClientTestDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_GO_ONLINE, &ClibGBT28181GUIClientTestDlg::OnClickedButtonGoOnline)
    ON_BN_CLICKED(IDC_BUTTON_QUERY_DEVICE_STATUS, &ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryDeviceStatus)
    ON_BN_CLICKED(IDC_BUTTON_QUERY_DEVICE_INFO, &ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryDeviceInfo)
    ON_BN_CLICKED(IDC_BUTTON_QUERY_CATALOG, &ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryCatalog)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE_INFO, &ClibGBT28181GUIClientTestDlg::OnClickedButtonUpdateInfo)
    ON_BN_CLICKED(IDC_BUTTON_GET_LIVE_VIDEO, &ClibGBT28181GUIClientTestDlg::OnClickedButtonGetLiveVideo)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE_VIDEO, &ClibGBT28181GUIClientTestDlg::OnClickedButtonCloseVideo)
    ON_BN_CLICKED(IDC_BUTTON_DOWN, &ClibGBT28181GUIClientTestDlg::OnClickedButtonDown)
    ON_BN_CLICKED(IDC_BUTTON_LEFT, &ClibGBT28181GUIClientTestDlg::OnClickedButtonLeft)
    ON_BN_CLICKED(IDC_BUTTON_LEFT_DOWN, &ClibGBT28181GUIClientTestDlg::OnClickedButtonLeftDown)
    ON_BN_CLICKED(IDC_BUTTON_LEFT_UP, &ClibGBT28181GUIClientTestDlg::OnClickedButtonLeftUp)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT, &ClibGBT28181GUIClientTestDlg::OnClickedButtonRight)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT_DOWN, &ClibGBT28181GUIClientTestDlg::OnClickedButtonRightDown)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT_UP, &ClibGBT28181GUIClientTestDlg::OnClickedButtonRightUp)
    ON_BN_CLICKED(IDC_BUTTON_UP, &ClibGBT28181GUIClientTestDlg::OnClickedButtonUp)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_IN, &ClibGBT28181GUIClientTestDlg::OnClickedButtonZoomIn)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_OUT, &ClibGBT28181GUIClientTestDlg::OnClickedButtonZoomOut)
END_MESSAGE_MAP()

void query_deviceInfo_callback(char* device_ID, char* device_type, char* manufacturer, char* model, char* firmware, uint64_t max_camera, uint64_t max_alarm)
{
    CString temp;
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output +=
        _T("device_ID   : ") + CString(device_ID) + _T("\r\n") +
        _T("device_type : ") + CString(device_type) + _T("\r\n") +
        _T("manufacturer: ") + CString(manufacturer) + _T("\r\n") +
        _T("model       : ") + CString(model) + _T("\r\n") +
        _T("firmware    : ") + CString(firmware) + _T("\r\n");
    temp.Format(_T("max_camera  : %u\r\n"), max_camera);
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += temp;
    temp.Format(_T("max_alarm   : %u\r\n\r\n"), max_alarm);
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += temp;
}

void query_device_status_callback(char* deviceID, MANSCDP_on_off_line online, MANSCDP_result_type status)
{
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("device_ID: ") + CString(deviceID) + _T("\r\n");
    switch(online)
    {
        case MANSCDP_ONLINE:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("online   : ONLINE\r\n");
            break;
        case MANSCDP_OFFLINE:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("online   : OFFLINE\r\n");
            break;
        case MANSCDP_UNKNOWN_ON_OFF_LINE:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("online   : UNKNOWN\r\n");
            break;
    }
    switch(status)
    {
        case MANSCDP_OK:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("status   : OK\r\n\r\n");
            break;
        case MANSCDP_ERROR:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("status   : ERROR\r\n\r\n");
            break;
        case MANSCDP_UNKNOWN_RESULT_TYPE:
            p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("status   : UNKNOWN\r\n\r\n");
            break;
    }
}

void query_device_catalog_callback(char* deviceID, uint64_t sum_num, MANSCDP_device* p_MANSCDP_device_array)
{
    CString temp;
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("device_ID: ") + CString(deviceID) + _T("\r\n");
    temp.Format(_T("count    : %u\r\n"), sum_num);
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += temp;
    for(uint64_t i = 0; i < sum_num; i++)
    {
        temp.Format(_T("device number: %u\r\n"), i + 1);
        p_global_ClibGBT28181GUIClientTestDlg->m_info_output += temp;
        p_global_ClibGBT28181GUIClientTestDlg->m_info_output +=
            _T("DeviceID    : ") + CString(p_MANSCDP_device_array[i].DeviceID) + _T("\r\n") +
            _T("Name        : ") + CString(p_MANSCDP_device_array[i].Name) + _T("\r\n") +
            _T("Manufacturer: ") + CString(p_MANSCDP_device_array[i].Manufacturer) + _T("\r\n") +
            _T("Model       : ") + CString(p_MANSCDP_device_array[i].Model) + _T("\r\n") +
            _T("Owner       : ") + CString(p_MANSCDP_device_array[i].Owner) + _T("\r\n") +
            _T("CivilCode   : ") + CString(p_MANSCDP_device_array[i].CivilCode) + _T("\r\n") +
            _T("Address     : ") + CString(p_MANSCDP_device_array[i].Address) + _T("\r\n") +
            _T("Parental    : ") + CString(p_MANSCDP_device_array[i].Parental) + _T("\r\n") +
            _T("SafetyWay   : ") + CString(p_MANSCDP_device_array[i].SafetyWay) + _T("\r\n") +
            _T("RegisterWay : ") + CString(p_MANSCDP_device_array[i].RegisterWay) + _T("\r\n") +
            _T("Secrecy     : ") + CString(p_MANSCDP_device_array[i].Secrecy) + _T("\r\n") +
            _T("Status      : ") + CString(p_MANSCDP_device_array[i].Status) + _T("\r\n");
    }
    p_global_ClibGBT28181GUIClientTestDlg->m_info_output += _T("\r\n");
}

// ClibGBT28181GUIClientTestDlg message handlers

#define PROGRAM_STREAM 0
FILE* p_global_file_PS_data;
FILE* p_global_file_PS_data_size;

BOOL ClibGBT28181GUIClientTestDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetWindowLong(this->m_hWnd, GWL_STYLE, GetWindowLong(this->m_hWnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_SYSMENU);
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    p_global_ClibGBT28181GUIClientTestDlg = this;
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
    m_maximun_time_out_number = 3;
    m_target_SIP_user_name = _T("34020000001320000144");
    m_target_IP = _T("192.168.10.144");
    m_target_port = 5060;
    m_pan_speed = 100;
    m_tilt_speed = 100;
    m_zoom_speed = 5;
    m_info_output = _T("All kinds of info out put area.\r\n\r\n");
    UpdateData(FALSE);

    int32_t result = GBT28181_client_initial();
    CString message;
    message.Format(_T("GBT28181_client_initial return: %d\r\n\r\n"), result);
    m_info_output += message;
    UpdateData(FALSE);

    initial_decode_DLL(64);

    p_global_file_PS_data = fopen("D:\\PSdata", "ab");
    p_global_file_PS_data_size = fopen("D:\\PSdataSize", "ab");

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
    CString message;
    if(GBT28181_SUCCESS != result)
    {
        message.Format(_T("GBT28181_free_client error: %d"), result);
        AfxMessageBox(message);
    }

    free_decode_DLL();

    fclose(p_global_file_PS_data);
    fclose(p_global_file_PS_data_size);

    Sleep(1000);

    return CDialogEx::DestroyWindow();
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonGoOnline()
{
    // TODO: Add your control notification handler code here
    CString message;
    int result = 0;

    GetDlgItem(IDC_EDIT_LOCAL_CLIENT_SIP_USER_NAME)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_LOCAL_CLIENT_SIP_USER_ID)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_LOCAL_CLIENT_SIP_PASSWORD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_LOCAL_CLIENT_IP)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_LOCAL_CLIENT_PORT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_SERVER_ID)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_SERVER_DOMAIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_SERVER_IP)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_SERVER_PORT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_REGISTER_EXPIRATION_INTERVAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_HEARTBEAT_INTERVAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_MAXIMUN_TIME_OUT_INTERVAL)->EnableWindow(FALSE);

    UpdateData();

    result = SetFunctionWithCharParameter(GBT28181_set_client_name, m_local_client_SIP_user_name);
    message.Format(_T("GBT28181_set_client_name return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_client_ID, m_local_client_SIP_user_ID);
    message.Format(_T("GBT28181_set_client_ID return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_client_password, m_local_client_SIP_password);
    message.Format(_T("GBT28181_set_client_password return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_client_IP, m_local_client_IP);
    message.Format(_T("GBT28181_set_client_IP return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_client_port(m_local_client_port);
    message.Format(_T("GBT28181_set_client_port return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_SIP_server_ID, m_SIP_server_ID);
    message.Format(_T("GBT28181_set_SIP_server_ID return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_SIP_server_domain, m_SIP_server_domain);
    message.Format(_T("GBT28181_set_SIP_server_domain return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = SetFunctionWithCharParameter(GBT28181_set_SIP_server_IP, m_SIP_server_IP);
    message.Format(_T("GBT28181_set_SIP_server_IP return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_SIP_server_port(m_SIP_server_port);
    message.Format(_T("GBT28181_set_SIP_server_port return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_expiration_interval(m_register_expiration_interval);
    message.Format(_T("GBT28181_set_expiration_interval return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_heartbeat_interval(m_heartbeat_interval);
    message.Format(_T("GBT28181_set_heartbeat_interval return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_max_time_out_number(m_maximun_time_out_number);
    message.Format(_T("GBT28181_set_max_time_out_number return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_address_family(GBT28181_AF_INET);
    message.Format(_T("GBT28181_set_address_family return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_IP_protocol(GBT28181_IPPROTO_UDP);
    message.Format(_T("GBT28181_set_IP_protocol return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_max_number_of_live_video(64);
    message.Format(_T("GBT28181_set_max_number_of_live_video return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_query_device_info_callback(query_deviceInfo_callback);
    message.Format(_T("GBT28181_set_query_device_info_callback return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_query_device_status_callback(query_device_status_callback);
    message.Format(_T("GBT28181_set_query_device_status_callback return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_set_query_catalog_callback(query_device_catalog_callback);
    message.Format(_T("GBT28181_set_query_catalog_callback return: %d\r\n\r\n"), result);
    m_info_output += message;

    result = GBT28181_client_go_online();
    message.Format(_T("GBT28181_client_go_online return: %d\r\n\r\n"), result);
    m_info_output += message;

    UpdateData(FALSE);
}


int ClibGBT28181GUIClientTestDlg::SetFunctionWithCharParameter(int(*p_function)(char*), CString& cstringToSet)
{
    int result = p_function(CCStringToChar(cstringToSet).GetCStyleString());
    return result;
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryDeviceStatus()
{
    // TODO: Add your control notification handler code here
    CString message;
    int result = 0;

    result = GBT28181_query_device_status(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        NULL,
        m_target_port);
    message.Format(_T("GBT28181_query_device_status return: %d\r\n\r\n"), result);
    m_info_output += message;

    UpdateData(FALSE);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryDeviceInfo()
{
    // TODO: Add your control notification handler code here
    CString message;
    int result = 0;

    result = GBT28181_query_device_info(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        NULL,
        m_target_port);
    message.Format(_T("GBT28181_query_device_info return: %d\r\n\r\n"), result);
    m_info_output += message;

    UpdateData(FALSE);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonQueryCatalog()
{
    // TODO: Add your control notification handler code here
    CString message;
    int result = 0;

    result = GBT28181_query_catalog(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        NULL,
        m_target_port);
    message.Format(_T("GBT28181_query_catalog return: %d\r\n\r\n"), result);
    m_info_output += message;

    UpdateData(FALSE);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonUpdateInfo()
{
    // TODO: Add your control notification handler code here
    UpdateData(FALSE);
}

int payload_callback(uint32_t session_handle, uint8_t* payload, uint32_t payload_size, uint16_t sequence_number, uint32_t timestamp)
{
    if(PROGRAM_STREAM)
    {
        fwrite(payload, payload_size, 1, p_global_file_PS_data);
        fwrite(&payload_size, 4, 1, p_global_file_PS_data_size);
        return 0;
    }
    else
    {
        return decode(global_instance, payload, payload_size, sequence_number, timestamp);
    }
}

void ClibGBT28181GUIClientTestDlg::OnClickedButtonGetLiveVideo()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    myparamInput param;
    param.fps = 25;
    param.height = 720;
    param.width = 1280;
    param.isDecode = true;
    param.playHandle = GetDlgItem(IDC_PICTURE_AREA)->m_hWnd;

    m_play_instance = get_idle_decode_instance();
    initial_decode_parameter(m_play_instance, &param, 3);
    set_decode_hardware_acceleration(m_play_instance, true);
    playing_windows_RECT_changed_of_decode_DLL(m_play_instance);
    global_instance = m_play_instance;

    GBT28181_get_idle_real_time_stream_handle((uint32_t*)&m_live_time_stream_handle);

    GBT28181_set_RTP_port(m_live_time_stream_handle, 6002);

    GBT28181_set_RTP_protocol(m_live_time_stream_handle, GBT28181_IPPROTO_UDP);

    GBT28181_set_RTP_payload_give_out_callback(m_live_time_stream_handle, payload_callback);

    GBT28181_get_real_time_stream(
        m_live_time_stream_handle,
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonCloseVideo()
{
    // TODO: Add your control notification handler code here
    free_decode_instance(global_instance);
    GBT28181_close_real_time_stream(m_live_time_stream_handle);
    RedrawWindow();
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonDown()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 1;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonLeft()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 1;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonLeftDown()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 1;
    control_data.left = 1;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonLeftUp()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 1;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 1;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonRight()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 1;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonRightDown()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 1;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 1;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonRightUp()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 1;
    control_data.up = 1;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonUp()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 1;
    control_data.zoom_in = 0;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonZoomIn()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 0;
    control_data.zoom_in = 1;
    control_data.zoom_out = 0;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}


void ClibGBT28181GUIClientTestDlg::OnClickedButtonZoomOut()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    PTZ_control_data control_data;
    control_data.down = 0;
    control_data.left = 0;
    control_data.relative_pan_speed = m_pan_speed;
    control_data.relative_tilt_speed = m_tilt_speed;
    control_data.relative_zoom_speed = m_zoom_speed;
    control_data.right = 0;
    control_data.up = 0;
    control_data.zoom_in = 0;
    control_data.zoom_out = 1;

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);

    memset(&control_data, 0x0, sizeof(PTZ_control_data));

    Sleep(PTZ_SLEEP);

    GBT28181_PTZ_control(
        CCStringToChar(m_target_SIP_user_name).GetCStyleString(),
        CCStringToChar(m_target_IP).GetCStyleString(),
        m_target_port,
        &control_data);
}
