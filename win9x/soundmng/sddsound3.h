/**
 * @file	sddsound3.h
 * @brief	DSound3 �I�[�f�B�I �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <map>
#include <vector>
#include <dsound.h>
#include "sdbase.h"
#include "misc\threadbase.h"

/**
 * @brief �f�o�C�X
 */
struct DSound3Device
{
	GUID guid;							//!< GUID
	TCHAR szDevice[MAX_PATH];			//!< �f�o�C�X
};

/**
 * @brief Direct Sound3 �N���X
 */
class CSoundDeviceDSound3 : public CSoundDeviceBase, protected CThreadBase
{
public:
	static void Initialize();
	static void EnumerateDevices(std::vector<LPCTSTR>& devices);

	CSoundDeviceDSound3();
	virtual ~CSoundDeviceDSound3();
	virtual bool Open(LPCTSTR lpDevice = NULL, HWND hWnd = NULL);
	virtual void Close();
	virtual UINT CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize = 0);
	virtual void DestroyStream();
	virtual void ResetStream();
	virtual bool PlayStream();
	virtual void StopStream();
	virtual bool LoadPCM(UINT nNum, LPCTSTR lpFilename = NULL);
	virtual void SetPCMVolume(UINT nNum, int nVolume);
	virtual bool PlayPCM(UINT nNum, BOOL bLoop);
	virtual void StopPCM(UINT nNum);
	virtual void StopAllPCM();

protected:
	virtual bool Task();

private:
	static std::vector<DSound3Device> sm_devices;	//!< �f�o�C�X ���X�g

	LPDIRECTSOUND m_lpDSound;					//!< Direct Sound �C���^�t�F�C�X
	LPDIRECTSOUNDBUFFER m_lpDSStream;			//!< �X�g���[�� �o�b�t�@
	UINT m_nChannels;							//!< �`���l����
	UINT m_nBufferSize;							//!< �o�b�t�@ �T�C�Y
	UINT m_dwHalfBufferSize;					//!< �o�b�t�@ �o�C�g
	HANDLE m_hEvents[2];						//!< �C�x���g
	std::map<UINT, LPDIRECTSOUNDBUFFER> m_pcm;	//!< PCM �o�b�t�@

	static BOOL CALLBACK EnumCallback(LPGUID lpGuid, LPCTSTR lpcstrDescription, LPCTSTR lpcstrModule, LPVOID lpContext);
	void FillStream(DWORD dwPosition);
	void UnloadPCM(UINT nNum);
	void DestroyAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};
