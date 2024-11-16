
// KeySimulatorDlg.h : header file
//

#pragma once


// CKeySimulatorDlg dialog
class CKeySimulatorDlg : public CDialogEx
{
// Construction
public:
	CKeySimulatorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KEYSIMULATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString textBox_Input;
	afx_msg void OnEnChangeEdit2();
	CString valDelay;
	afx_msg void OnBnClickedSetstartkey();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDebugbtn();
	afx_msg void OnBnClickedLoop();
	afx_msg void OnBnClickedStart();
};
