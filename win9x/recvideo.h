/**
 * @file	recvideo.h
 * @brief	�^��N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#if defined(SUPPORT_RECVIDEO)

#include <vfw.h>

// #define AVI_SPLIT_SIZE		(1024 * 1024 * 1024)		/**< �����T�C�Y */

/**
 * @brief �^��N���X
 */
class RecodeVideo
{
public:
	static RecodeVideo& GetInstance();

	RecodeVideo();
	~RecodeVideo();
	bool Open(HWND hWnd, LPCTSTR lpFilename);
	void Close();
	void Write();
	void Update();
	bool IsEnabled() const;

private:
	static RecodeVideo sm_instance;		/**< �B��̃C���X�^���X�ł� */

	bool m_bEnabled;					/**< �L���t���O */
	bool m_bDirty;						/**< �_�[�e�B �t���O */

	int m_nStep;						/**< �N���b�N */
	UINT8* m_pWork8;					/**< ���[�N */
	UINT8* m_pWork24;					/**< ���[�N */

	PAVIFILE m_pAvi;					/**< AVIFILE */
	PAVISTREAM m_pStm;					/**< AVISTREAM */
	PAVISTREAM m_pStmTmp;				/**< AVISTREAM */
	UINT m_nFrame;						/**< �t���[���� */

	BITMAPINFOHEADER m_bmih;			/**< BITMAPINFOHEADER */
	COMPVARS m_cv;						/**< COMPVARS */

#if defined(AVI_SPLIT_SIZE)
	int m_nNumber;						/**< �t�@�C���ԍ� */
	DWORD m_dwSize;						/**< �T�C�Y */
	TCHAR m_szPath[MAX_PATH];			/**< �x�[�X �p�X */
#endif	// defined(AVI_SPLIT_SIZE)

	bool OpenFile(LPCTSTR lpFilename);
	void CloseFile();
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline RecodeVideo& RecodeVideo::GetInstance()
{
	return sm_instance;
}

/**
 * �L��?
 * @retval true �L��
 * @retval false ����
 */
inline bool RecodeVideo::IsEnabled() const
{
	return m_bEnabled;
}

#define recvideo_open			RecodeVideo::GetInstance().Open
#define recvideo_close			RecodeVideo::GetInstance().Close
#define recvideo_write			RecodeVideo::GetInstance().Write
#define recvideo_update			RecodeVideo::GetInstance().Update
#define recvideo_isEnabled		RecodeVideo::GetInstance().IsEnabled

#else	// defined(SUPPORT_RECVIDEO)

static inline bool recvideo_open(HWND hWnd, LPCTSTR f) { return false; }
static inline void recvideo_close() { }
static inline void recvideo_write() { }
static inline void recvideo_update() { }
static inline bool recvideo_isEnabled() { return false; }

#endif	// defined(SUPPORT_RECVIDEO)
