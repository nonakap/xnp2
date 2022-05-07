/**
 *	@file	np2arg.h
 *	@brief	�������N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �������N���X
 */
class Np2Arg
{
public:
	static Np2Arg* GetInstance();

	Np2Arg();
	~Np2Arg();
	void Parse();
	void ClearDisk();
	LPCTSTR disk(int nDrive) const;
	LPCTSTR iniFilename() const;
	bool fullscreen() const;

private:
	static Np2Arg sm_instance;		//!< �B��̃C���X�^���X�ł�

	LPCTSTR m_lpDisk[4];	//!< �f�B�X�N
	LPCTSTR m_lpIniFile;	//!< �ݒ�t�@�C��
	bool m_fFullscreen;		//!< �t���X�N���[�� ���[�h
	LPTSTR m_lpArg;			//!< ���[�N
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline Np2Arg* Np2Arg::GetInstance()
{
	return &sm_instance;
}

/**
 * �f�B�X�N �p�X�𓾂�
 * @param[in] nDrive �h���C�u
 * @return �f�B�X�N �p�X
 */
inline LPCTSTR Np2Arg::disk(int nDrive) const
{
	return m_lpDisk[nDrive];
}

/**
 * INI �p�X�𓾂�
 * @return INI �p�X
 */
inline LPCTSTR Np2Arg::iniFilename() const
{
	return m_lpIniFile;
}

/**
 * �t���X�N���[�����𓾂�
 * @retval true �t���X�N���[�� ���[�h
 * @retval false �E�B���h�E ���[�h
 */
inline bool Np2Arg::fullscreen() const
{
	return m_fFullscreen;
}
