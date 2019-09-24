#ifndef _C_SETTINGDLG_H_
#define _C_SETTINGDLG_H_

#include "Dialog.h"

class C_JubmooWnd;

class C_SettingDlg : public C_Dialog
{
public:
	C_SettingDlg(C_JubmooWnd * pParentWnd);
	~C_SettingDlg();

	void UpdateCtlBkg(HWND hCtl);

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

	void OnFaceSelect(int id=-1);
	void OnChangeSaveSlot(int nSlot);
	void OnChangeAISpeed(int nSlot);
	void OnChangePickSpeed(int nSlot);
	void OnResetScore();
	void OnResetComputerScore();
	void OnBmpSelect(int ctlid);
    void OnOK();

private:
	int m_nSelectedSlot;
	int m_nSelectedFace;
	int m_nSlotID[5];

	int m_nSelectedAISpeed;
	int m_nSelectedPickSpeed;
	int m_nOldAISpeed;
	int m_nOldPickSpeed;
};

#endif