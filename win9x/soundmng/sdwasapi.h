/**
 * @file	sdwasapi.h
 * @brief	WASAPI �I�[�f�B�I �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <vector>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include "sdbase.h"
#include "misc\threadbase.h"

/**
 * @brief �f�o�C�X
 */
struct WasapiDevice
{
	LPWSTR id;							//!< ID
	TCHAR szDevice[MAX_PATH];			//!< �f�o�C�X
};

/**
 * @brief WASAPI �N���X
 */
class CSoundDeviceWasapi : public CSoundDeviceBase, protected CThreadBase
{
public:
	static void Initialize();
	static void Deinitialize();
	static void EnumerateDevices(std::vector<LPCTSTR>& devices);

	CSoundDeviceWasapi();
	virtual ~CSoundDeviceWasapi();
	virtual bool Open(LPCTSTR lpDevice = NULL, HWND hWnd = NULL);
	virtual void Close();
	virtual UINT CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize = 0);
	virtual void DestroyStream();
	virtual bool PlayStream();
	virtual void StopStream();

protected:
	virtual bool Task();

private:
	static std::vector<WasapiDevice> sm_devices;	//!< �f�o�C�X ���X�g

	IMMDeviceEnumerator* m_pEnumerator;			//!< �f�o�C�X�񋓃C���X�^���X
	IMMDevice* m_pDevice;						//!< �f�o�C�X �C���X�^���X
	IAudioClient* m_pAudioClient;				//!< �I�[�f�B�I �N���C�A���g �C���X�^���X
	WAVEFORMATEX* m_pwfx;						//!< �t�H�[�}�b�g
	IAudioRenderClient* m_pRenderClient;		//!< �I�[�f�B�I �����_���[ �N���C�A���g �C���X�^���X
	UINT32 m_nBufferSize;						//!< �o�b�t�@ �T�C�Y
	HANDLE m_hEvents[2];						//!< �C�x���g
	void ResetStream();
	void FillStream();
};
