﻿/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "TemperatureDlg.h"

static CDiskInfoDlg* p;

IMPLEMENT_DYNAMIC(CTemperatureDlg, CDialog)

CTemperatureDlg::CTemperatureDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CTemperatureDlg::IDD, pParent)
{
	p = (CDiskInfoDlg*)pParent;

	m_ZoomType = p->GetZoomType();
	m_FontScale = p->GetFontScale();
	m_FontRatio = p->GetFontRatio();
	m_FontFace = p->GetFontFace();
	m_CurrentLangPath = p->GetCurrentLangPath();
	m_DefaultLangPath = p->GetDefaultLangPath();
	m_ThemeDir = p->GetThemeDir();
	m_CurrentTheme = p->GetCurrentTheme();
	m_DefaultTheme = p->GetDefaultTheme();
	m_Ini = p->GetIniPath();
}

CTemperatureDlg::~CTemperatureDlg()
{
}

void CTemperatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VALUE_TEMPERATURE, m_ValueTemperature);
	DDX_Text(pDX, IDC_VALUE_TEMPERATURE_F, m_ValueTemperatureF);
	DDX_Control(pDX, IDC_SCROLLBAR_TEMPERATURE, m_CtrlScrollbarTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE, m_CtrlValueTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE_F, m_CtrlValueTemperatureF);
	DDX_Control(pDX, IDC_SELECT_DISK, m_CtrlSelectDisk);
	DDX_Control(pDX, IDC_APPLY, m_CtrlApply);
	DDX_Control(pDX, IDC_DEFAULT, m_CtrlDefault);
}

BEGIN_MESSAGE_MAP(CTemperatureDlg, CDialogFx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY, &CTemperatureDlg::OnApply)
	ON_BN_CLICKED(IDC_DEFAULT, &CTemperatureDlg::OnDefault)
	ON_CBN_SELCHANGE(IDC_SELECT_DISK, &CTemperatureDlg::OnCbnSelchangeSelectDisk)
END_MESSAGE_MAP()

BOOL CTemperatureDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowText(i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")));

	m_CtrlScrollbarTemperature.SetScrollRange(20, 80);

	m_bShowWindow = TRUE;

	InitLang();
	InitSelectDisk();

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	if (p->m_Ata.vars.GetCount() == 0)
	{
		m_CtrlSelectDisk.EnableWindow(FALSE);
		m_CtrlScrollbarTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperatureF.EnableWindow(FALSE);
		m_CtrlApply.EnableWindow(FALSE);
		m_CtrlDefault.EnableWindow(FALSE);
	}

	return TRUE;
}

void CTemperatureDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	ChangeZoomType(m_ZoomType);
	SetClientSize((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);
	UpdateBackground(FALSE, m_bDarkMode);

	m_CtrlScrollbarTemperature.MoveWindow((DWORD)(8 * m_ZoomRatio), (DWORD)(44 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));
	m_CtrlValueTemperature.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio);
   	m_CtrlValueTemperatureF.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio);
	m_CtrlValueTemperature.InitControl(292, 44, 48, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode);
	m_CtrlValueTemperatureF.InitControl(344, 44, 48, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode);

	m_CtrlApply.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio);

	m_CtrlApply.InitControl(220, 80, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode);
	m_CtrlDefault.InitControl(20, 80, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode);

	m_CtrlSelectDisk.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio);
	m_CtrlSelectDisk.InitControl(8, 8, 384, 40, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_CtrlSelectDisk.SetMargin(0, 4, 0, 0, m_ZoomRatio);

	m_CtrlValueTemperature.SetDrawFrame(TRUE);
	m_CtrlValueTemperatureF.SetDrawFrame(TRUE);

	SetDarkModeControl(m_CtrlApply.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlDefault.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlScrollbarTemperature.GetSafeHwnd(), m_bDarkMode);

	// SetLayeredWindow(m_CtrlSelectDisk.GetListHwnd(), m_ComboAlpha);
	for (int i = -1; i < m_CtrlSelectDisk.GetCount(); i++)
	{
		m_CtrlSelectDisk.SetItemHeightEx(i, 24, m_ZoomRatio, m_FontRatio);
	}

	Invalidate();
}

void CTemperatureDlg::InitLang()
{
	m_CtrlApply.SetWindowTextW(i18n(_T("HealthStatus"), _T("APPLY")));
	m_CtrlDefault.SetWindowTextW(i18n(_T("HealthStatus"), _T("DEFAULT")));
}

void CTemperatureDlg::InitSelectDisk()
{
	CString select;
	CString cstr;

	if (p->m_Ata.vars.GetCount() <= 0)
	{
		return;
	}

	for (int i = 0; i < p->m_Ata.vars.GetCount(); i++)
	{
		CString temp;
		if (p->m_Ata.vars[i].IsSsd)
		{
			temp = _T("[SSD]");
		}

		CString driveLetter;
		if (p->m_Ata.vars[i].DriveMap.IsEmpty())
		{
			if (p->m_Ata.vars[i].PhysicalDriveId >= 0)
			{
				driveLetter.Format(_T("(Disk %d)"), p->m_Ata.vars[i].PhysicalDriveId);
			}
		}
		else
		{
			driveLetter.Format(_T("(%s)"), p->m_Ata.vars[i].DriveMap);
		}
		cstr.Format(_T("(%d) %s %s %s"), i + 1, p->m_Ata.vars.GetAt(i).Model, temp, driveLetter);

		m_CtrlSelectDisk.AddString(cstr);
	}
	m_CtrlSelectDisk.SetCurSel(0);

	UpdateData(TRUE);
	UpdateSelectDisk(0);

	UpdateData(TRUE);
	m_DiskIndex = 0;
	UpdateSelectDisk(m_DiskIndex);
}


void CTemperatureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int position = pScrollBar->GetScrollPos();
	switch (nSBCode)
	{

	case SB_LINELEFT:
		position -= 1;
		break;
	case SB_LINERIGHT:
		position += 1;
		break;
	case SB_PAGELEFT:
		position -= 5;
		break;
	case SB_PAGERIGHT:
		position += 5;
		break;
	case SB_LEFT:
		break;
	case SB_RIGHT:
		break;
	case SB_THUMBTRACK:
		position = nPos;
		break;
	}
	pScrollBar->SetScrollPos(position);

	CString cstr;
	if (*pScrollBar == m_CtrlScrollbarTemperature)
	{
		m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
		m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);
	}

	UpdateData(FALSE);

	CDialogFx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTemperatureDlg::UpdateSelectDisk(DWORD index)
{
	if (p->m_Ata.vars[index].IsSsd)
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileInt(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 60, m_Ini));
	}
	else
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileInt(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 50, m_Ini));
	}
	m_CtrlScrollbarTemperature.EnableWindow(TRUE);

	m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
	m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);
	UpdateData(FALSE);
}

void CTemperatureDlg::OnApply()
{
	UpdateData(TRUE);

	WritePrivateProfileString(_T("AlarmTemperature"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueTemperature, m_Ini);
	p->m_Ata.vars[m_DiskIndex].AlarmTemperature = _tstoi(m_ValueTemperature);
	p->SendMessage(WM_COMMAND, ID_REFRESH);
}

void CTemperatureDlg::OnDefault()
{
	if (p->m_Ata.vars[m_DiskIndex].IsSsd)
	{
		m_CtrlScrollbarTemperature.SetScrollPos(60);
	}
	else
	{
		m_CtrlScrollbarTemperature.SetScrollPos(50);
	}
	
	m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
	m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);

	UpdateData(FALSE);
}

void CTemperatureDlg::OnCbnSelchangeSelectDisk()
{
	if (m_DiskIndex != m_CtrlSelectDisk.GetCurSel())
	{
		m_DiskIndex = m_CtrlSelectDisk.GetCurSel();
		UpdateSelectDisk(m_DiskIndex);
	}
}
