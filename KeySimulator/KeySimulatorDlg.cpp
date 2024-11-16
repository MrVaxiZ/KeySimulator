
// KeySimulatorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KeySimulator.h"
#include "KeySimulatorDlg.h"
#include "afxdialogex.h"
#include "thread"
#include <atomic>

// Internal includes
#include "KeyHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Global variables
HHOOK g_hKeyboardHook = NULL;      // Keyboard hook handle
UINT  g_activationVkCode = 0;      // Virtual key code of the activation key

// Add these global variables
std::thread g_workerThread;				  // Thread object
std::atomic<bool> g_threadRunning(false); // Indicates if the thread is running
std::atomic<bool> g_stopThread(false);    // Signal to stop the thread

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			UINT vkCode = pKeyboard->vkCode;

			if (vkCode == g_activationVkCode)
			{
				// Toggle the thread running state
				if (!g_threadRunning)
				{
					g_stopThread = false;
					g_threadRunning = true;

					// Start the worker thread
					g_workerThread = std::thread([]() {
						KeyHandler keyhandler(0);

						do
						{
							keyhandler.instance->SimulateKeyPress(StaticData::textBox_Input_p);
							if (StaticData::delay > 0.0f) {
								Sleep(static_cast<DWORD>(StaticData::delay)); // Ensure delay is in milliseconds
							}

							if (!StaticData::checkBox_Loop_Clicked)
							{
								break;
							}
						} while (!g_stopThread);

						g_threadRunning = false;
						});
				}
				else
				{
					// Signal the thread to stop
					g_stopThread = true;
					if (g_workerThread.joinable())
					{
						g_workerThread.join();
					}
					g_threadRunning = false;
				}
			}
		}
	}
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKeySimulatorDlg dialog


CKeySimulatorDlg::CKeySimulatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KEYSIMULATOR_DIALOG, pParent)
	, textBox_Input(_T(""))
	, valDelay(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeySimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, textBox_Input);
	DDX_Text(pDX, IDC_EDIT2, valDelay);
}

BEGIN_MESSAGE_MAP(CKeySimulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CKeySimulatorDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_SetStartKey, &CKeySimulatorDlg::OnBnClickedSetstartkey)
	ON_BN_CLICKED(IDOK, &CKeySimulatorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DebugBtn, &CKeySimulatorDlg::OnBnClickedDebugbtn)
	ON_BN_CLICKED(IDLOOP, &CKeySimulatorDlg::OnBnClickedLoop)
	ON_EN_CHANGE(IDC_EDIT2, &CKeySimulatorDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_START, &CKeySimulatorDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CKeySimulatorDlg message handlers

BOOL CKeySimulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

void CKeySimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeySimulatorDlg::OnPaint()
{
	if (IsIconic())
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
HCURSOR CKeySimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKeySimulatorDlg::OnEnChangeEdit1()
{
	UpdateData(TRUE);

	StaticData::textBox_Input_p = &textBox_Input;
}


void CKeySimulatorDlg::OnEnChangeEdit2()
{
	UpdateData(TRUE);

	CString temp = valDelay;
	CString filteredStr;

	for (int i = 0; i < temp.GetLength(); i++)
	{
		TCHAR ch = temp[i];
		if (_istdigit(ch) || ch == _T('.'))
		{
			filteredStr += ch;
		}
	}

	float value = static_cast<float>(_tstof(filteredStr));

	StaticData::delay = value;
}


void CKeySimulatorDlg::OnBnClickedSetstartkey() {
	KeyHandler keyhandler(IDC_SetStartKey);
	keyhandler.SetHook();

	AfxMessageBox(_T("Press any key or mouse button to set the start key."));
}
 

void CKeySimulatorDlg::OnBnClickedOk()
{
	// TODO: Add data save
	CDialogEx::OnOK();
}

void CKeySimulatorDlg::OnBnClickedDebugbtn()
{
	CString message;
	message.Format(_T("=== Static Data Output ===\n")
		_T("checkBox_Loop_Clicked : %s\n")
		_T("btn_Start_Clicked : %s\n")
		_T("key_detected : %s\n")
		_T("textBox_Input_p : %s\n")
		_T("activationKey : %s\n")
		_T("delayVal : %f\n"),
		StaticData::checkBox_Loop_Clicked ? _T("true") : _T("false"),
		StaticData::btn_Start_Clicked ? _T("true") : _T("false"),
		StaticData::key_detected ? _T("true") : _T("false"),
		StaticData::textBox_Input_p ? *(StaticData::textBox_Input_p) : CString(_T("nullptr")),
		StaticData::activationKey,
		StaticData::delay);

	AfxMessageBox(message);
}

void CKeySimulatorDlg::OnBnClickedLoop()
{
	StaticData::checkBox_Loop_Clicked = !StaticData::checkBox_Loop_Clicked ? true : false;
}

void CKeySimulatorDlg::OnBnClickedStart()
{
	if (!StaticData::activationKey.IsEmpty())
	{
		StaticData::btn_Start_Clicked = !StaticData::btn_Start_Clicked ? true : false;
	}
	else
	{
		AfxMessageBox(_T("Activation key is not set."));
		return;
	}

	CWnd* pButton = AfxGetMainWnd()->GetDlgItem(IDC_START);

	if (!StaticData::btn_Start_Clicked && pButton != nullptr && !StaticData::activationKey.IsEmpty()) {
		pButton->SetWindowText(_T("Start"));
	}
	else {
		pButton->SetWindowText(_T("Press [") + StaticData::activationKey + ("] to start sending data"));
	}

	// Get the first character of the activation key
	TCHAR ch = StaticData::activationKey[0];

	// Convert character to virtual key code
	SHORT s = VkKeyScanEx(ch, GetKeyboardLayout(0));
	g_activationVkCode = s & 0xFF;  // Extract the virtual key code

	// Install the low-level keyboard hook
	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	if (g_hKeyboardHook == NULL)
	{
		AfxMessageBox(_T("Failed to install keyboard hook!"));
	}
}