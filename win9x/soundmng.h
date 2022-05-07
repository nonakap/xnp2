/**
 * @file	soundmng.h
 * @brief	�T�E���h �}�l�[�W�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * PCM �ԍ�
 */
enum SoundPCMNumber
{
	SOUND_PCMSEEK		= 0,		/*!< �w�b�h�ړ� */
	SOUND_PCMSEEK1					/*!< 1�N���X�^�ړ� */
};

#ifdef __cplusplus
extern "C"
{
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
#define soundmng_sync()
void soundmng_setreverse(BOOL bReverse);

BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop);
void soundmng_pcmstop(enum SoundPCMNumber nNum);

#ifdef __cplusplus
}

#include "soundmng\sdbase.h"

/**
 * �T�E���h �v���V�[�W��
 */
enum SoundProc
{
	SNDPROC_MASTER		= 0,
	SNDPROC_MAIN,
	SNDPROC_TOOL,
	SNDPROC_SUBWIND
};

/**
 * @brief �T�E���h �}�l�[�W�� �N���X
 */
class CSoundMng : public ISoundData
{
public:
	/**
	 * �f�o�C�X �^�C�v
	 */
	enum DeviceType
	{
		kDefault			= 0,	/*!< Default */
		kDSound3,					/*!< Direct Sound3 */
		kWasapi,					/*!< WASAPI */
		kAsio						/*!< ASIO */
	};

	static CSoundMng* GetInstance();
	static void Initialize();
	static void Deinitialize();

	CSoundMng();
	bool Open(DeviceType nType, LPCTSTR lpName, HWND hWnd);
	void Close();
	void Enable(SoundProc nProc);
	void Disable(SoundProc nProc);
	UINT CreateStream(UINT nSamplingRate, UINT ms);
	void DestroyStream();
	void ResetStream();
	void PlayStream();
	void StopStream();
	void SetReverse(bool bReverse);
	void LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename);
	void SetPCMVolume(SoundPCMNumber nNum, int nVolume);
	bool PlayPCM(SoundPCMNumber nNum, BOOL bLoop);
	void StopPCM(SoundPCMNumber nNum);
	virtual UINT Get16(SINT16* lpBuffer, UINT nBufferCount);

private:
	static CSoundMng sm_instance;		//!< �B��̃C���X�^���X�ł�

	/**
	 * satuation�֐��^�錾
	 */
	typedef void (PARTSCALL * FNMIX)(SINT16*, const SINT32*, UINT);

	CSoundDeviceBase* m_pSoundDevice;	//!< �T�E���h �f�o�C�X
	UINT m_nMute;						//!< �~���[�g �t���O
	FNMIX m_fnMix;						//!< satuation�֐��|�C���^
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline CSoundMng* CSoundMng::GetInstance()
{
	return &sm_instance;
}

#endif
