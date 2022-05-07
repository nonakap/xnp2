/**
 * @file	c_midi.cpp
 * @brief	MIDI �R���g���[�� �N���X�Q�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "c_midi.h"
#include "commng\cmmidi.h"
#if defined(SUPPORT_VSTi)
#include "commng\cmmidioutvst.h"
#endif	// defined(SUPPORT_VSTi)
#if defined(MT32SOUND_DLL)
#include "..\ext\mt32snd.h"
#endif	// defined(MT32SOUND_DLL)

/**
 * MIDI �f�o�C�X�̏�����
 */
void CComboMidiDevice::PreSubclassWindow()
{
	// N/C
	std::tstring rNC(LoadTString(IDS_NONCONNECT));
	AddString(rNC.c_str());
}

/**
 * MIDI IN �f�o�C�X�̗�
 */
void CComboMidiDevice::EnumerateMidiIn()
{
	const UINT nDevs = ::midiInGetNumDevs();
	for (UINT i = 0; i < nDevs; i++)
	{
		MIDIINCAPS mic;
		if (::midiInGetDevCaps(i, &mic, sizeof(mic)) == MMSYSERR_NOERROR)
		{
			AddString(mic.szPname);
		}
	}
}

/**
 * MIDI OUT �f�o�C�X�̗�
 */
void CComboMidiDevice::EnumerateMidiOut()
{
	// MIDI MAPPER
	AddString(cmmidi_midimapper);

	// Vermouth
#if defined(VERMOUTH_LIB)
	AddString(cmmidi_vermouth);
#endif	// defined(VERMOUTH_LIB)

	// MT32Sound
#if defined(MT32SOUND_DLL)
	if (MT32Sound::GetInstance()->IsEnabled())
	{
		AddString(cmmidi_mt32sound);
	}
#endif	// defined(MT32SOUND_DLL)

#if defined(SUPPORT_VSTi)
	if (CComMidiOutVst::IsEnabled())
	{
		AddString(cmmidi_midivst);
	}
#endif	// defined(SUPPORT_VSTi)

	const UINT nDevs = ::midiOutGetNumDevs();
	for (UINT i = 0; i <nDevs; i++)
	{
		MIDIOUTCAPS moc;
		if (::midiOutGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR)
		{
			AddString(moc.szPname);
		}
	}
}

/**
 * �J�[�\���ݒ�
 * @param[in] lpDevice �f�o�C�X��
 */
void CComboMidiDevice::SetCurString(LPCTSTR lpDevice)
{
	int nIndex = FindStringExact(-1, lpDevice);
	if (nIndex == CB_ERR)
	{
		nIndex = 0;
	}
	SetCurSel(nIndex);
}

/**
 * MIDI ���W���[����
 */
void CComboMidiModule::PreSubclassWindow()
{
	for (UINT i = 0; i < _countof(cmmidi_mdlname); i++)
	{
		AddString(cmmidi_mdlname[i]);
	}
}

/**
 * �t�@�C���I��
 */
void CEditMimpiFile::Browse()
{
	TCHAR szPath[MAX_PATH];
	GetWindowText(szPath, _countof(szPath));

	std::tstring rExt(LoadTString(IDS_MIMPIEXT));
	std::tstring rFilter(LoadTString(IDS_MIMPIFILTER));
	std::tstring rTitle(LoadTString(IDS_MIMPITITLE));

	CFileDlg dlg(TRUE, rExt.c_str(), szPath, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, rFilter.c_str(), m_hWnd);
	dlg.m_ofn.lpstrTitle = rTitle.c_str();
	dlg.m_ofn.nFilterIndex = 1;
	if (dlg.DoModal())
	{
		SetWindowText(dlg.GetPathName());
	}
	else
	{
		SetWindowText(TEXT(""));
	}
}
