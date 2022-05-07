/**
 * @file	usbdev.h
 * @brief	USB �A�N�Z�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WINXP
#include <WinUsb.h>

/**
 * @brief USB �A�N�Z�X �N���X
 */
class CUsbDev
{
public:
	CUsbDev();
	~CUsbDev();
	bool Open(unsigned int vid, unsigned int pid, unsigned int nIndex = 0);
	void Close();
	int CtrlXfer(int nType, int nRequest, int nValue = 0, int nIndex = 0, void* lpBuffer = NULL, int cbBuffer = 0);
	int WriteBulk(const void* lpBuffer, int cbBuffer);
	int ReadBulk(void* lpBuffer, int cbBuffer);
	bool IsOpened() const;

private:
	HANDLE m_hDev;						/*!< �f�o�C�X �n���h�� */
	WINUSB_INTERFACE_HANDLE m_hWinUsb;	/*!< WinUSB */
	UCHAR m_cOutPipeId;					/*!< �p�C�v ID */
	UCHAR m_cInPipeId;					/*!< �p�C�v id */
	static LPTSTR GetDevicePath(const GUID& InterfaceGuid, LPTSTR lpDevicePath, int cchDevicePath);
	bool Open(const GUID& InterfaceGuid);
	bool OpenDevice(LPCTSTR lpDevicePath);
};

/**
 * �I�[�v����?
 * @retval true �I�[�v����
 * @retval false ���I�[�v��
 */
inline bool CUsbDev::IsOpened() const
{
	return (m_hDev != INVALID_HANDLE_VALUE);
}
