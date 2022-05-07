/**
 * @file	sdasio.h
 * @brief	ASIO �I�[�f�B�I �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "asio\asiosdk.h"
#include "asio\asiodriverlist.h"
#include "sdbase.h"

/**
 * @brief ASIO �I�[�f�B�I �N���X
 */
class CSoundDeviceAsio : public CSoundDeviceBase
{
public:
	static void Initialize();
	static void EnumerateDevices(std::vector<LPCTSTR>& devices);

	CSoundDeviceAsio();
	virtual ~CSoundDeviceAsio();
	virtual bool Open(LPCTSTR lpDevice = NULL, HWND hWnd = NULL);
	virtual void Close();
	virtual UINT CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize = 0);
	virtual void DestroyStream();
	virtual bool PlayStream();
	virtual void StopStream();

private:
	static CSoundDeviceAsio* sm_pInstance;			/*!< ���݂̃C���X�^���X */
	IASIO* m_pAsioDriver;							/*!< ASIO �h���C�o */
	UINT m_nBufferLength;							/*!< �o�b�t�@ �T�C�Y */
	std::vector<ASIOBufferInfo> m_bufferInfo;		/*!< �o�b�t�@ */
	ASIOCallbacks m_callback;						/*!< �R�[���o�b�N */
	static AsioDriverList sm_asioDriverList;		/*!< �h���C�o ���X�g */
	static void cBufferSwitch(long doubleBufferIndex, ASIOBool directProcess);
	static void cSampleRateDidChange(ASIOSampleRate sRate);
	static long cAsioMessage(long selector, long value, void* message, double* opt);
	static ASIOTime* cBufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
	void BufferSwitch(long doubleBufferIndex, ASIOBool directProcess);
};
