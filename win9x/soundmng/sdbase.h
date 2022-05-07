/**
 * @file	sdbase.h
 * @brief	�T�E���h �f�o�C�X���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �T�E���h �f�[�^�擾�C���^�t�F�C�X
 */
class ISoundData
{
public:
	/**
	 * �X�g���[�� �f�[�^�𓾂�
	 * @param[out] lpBuffer �o�b�t�@
	 * @param[in] nBufferCount �o�b�t�@ �J�E���g
	 * @return �T���v����
	 */
	virtual UINT Get16(SINT16* lpBuffer, UINT nBufferCount) = 0;
};

/**
 * @brief �T�E���h �f�o�C�X���N���X
 */
class CSoundDeviceBase
{
public:
	/**
	 * �R���X�g���N�^
	 */
	CSoundDeviceBase()
		: m_pSoundData(NULL)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~CSoundDeviceBase()
	{
	}

	/**
	 * �X�g���[�� �f�[�^�̐ݒ�
	 * @param[in] pSoundData �T�E���h �f�[�^
	 */
	void SetStreamData(ISoundData* pSoundData)
	{
		m_pSoundData = pSoundData;
	}

	/**
	 * �I�[�v��
	 * @param[in] lpDevice �f�o�C�X��
	 * @param[in] hWnd �E�B���h�E �n���h��
	 * @retval true ����
	 * @retval false ���s
	 */
	virtual bool Open(LPCTSTR lpDevice = NULL, HWND hWnd = NULL) = 0;

	/**
	 * �N���[�Y
	 */
	virtual void Close() = 0;

	/**
	 * �X�g���[���̍쐬
	 * @param[in] nSamplingRate �T���v�����O ���[�g
	 * @param[in] nChannels �`���l����
	 * @param[in] nBufferSize �o�b�t�@ �T�C�Y
	 * @return �o�b�t�@ �T�C�Y
	 */
	virtual UINT CreateStream(UINT nSamplingRate, UINT nChannels, UINT nBufferSize = 0) = 0;

	/**
	 * �X�g���[����j��
	 */
	virtual void DestroyStream() = 0;

	/**
	 * �X�g���[�������Z�b�g
	 */
	virtual void ResetStream()
	{
	}

	/**
	 * �X�g���[���̍Đ�
	 * @retval true ����
	 * @retval false ���s
	 */
	virtual bool PlayStream() = 0;

	/**
	 * �X�g���[���̒�~
	 */
	virtual void StopStream() = 0;

	/**
	 * PCM �f�[�^�ǂݍ���
	 * @param[in] nNum PCM �ԍ�
	 * @param[in] lpFilename �t�@�C����
	 * @retval true ����
	 * @retval false ���s
	 */
	virtual bool LoadPCM(UINT nNum, LPCTSTR lpFilename)
	{
		return false;
	}

	/**
	 * PCM ���A�����[�h
	 * @param[in] nNum PCM �ԍ�
	 */
	virtual void UnloadPCM(UINT nNum)
	{
	}

	/**
	 * PCM ���H�����[���ݒ�
	 * @param[in] nNum PCM �ԍ�
	 * @param[in] nVolume ���H�����[��
	 */
	virtual void SetPCMVolume(UINT nNum, int nVolume)
	{
	}

	/**
	 * PCM �Đ�
	 * @param[in] nNum PCM �ԍ�
	 * @param[in] bLoop ���[�v �t���O
	 * @retval true ����
	 * @retval false ���s
	 */
	virtual bool PlayPCM(UINT nNum, BOOL bLoop)
	{
		return false;
	}

	/**
	 * PCM ��~
	 * @param[in] nNum PCM �ԍ�
	 */
	virtual void StopPCM(UINT nNum)
	{
	}

	/**
	 * PCM ���X�g�b�v
	 */
	virtual void StopAllPCM()
	{
	}

protected:
	ISoundData* m_pSoundData;		/*!< �T�E���h �f�[�^ �C���X�^���X */
};
