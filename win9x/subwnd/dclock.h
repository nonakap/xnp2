/**
 * @file	dclock.h
 * @brief	�����\���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

enum
{
	DCLOCK_WIDTH	= 56,
	DCLOCK_HEIGHT	= 12,
	DCLOCK_YALIGN	= (56 / 8)
};

struct DispClockPattern;

/**
 * @brief �����\���N���X
 */
class DispClock
{
public:
	static DispClock* GetInstance();

	DispClock();
	void Initialize();
	void SetPalettes(UINT bpp);
	const RGB32* GetPalettes() const;
	void Reset();
	void Update();
	void Redraw();
	bool IsDisplayed() const;
	void CountDown(UINT nFrames);
	bool Make();
	void Draw(UINT nBpp, void* lpBuffer, int nYAlign) const;

private:
	static DispClock sm_instance;		//!< �B��̃C���X�^���X�ł�

	/**
	 * @brief QuadBytes
	 */
	union QuadBytes
	{
		UINT8 b[8];			//!< bytes
		UINT64 q;			//!< quad
	};

	const DispClockPattern* m_pPattern;	//!< �p�^�[��
	QuadBytes m_nCounter;				//!< �J�E���^
	UINT8 m_cTime[8];					//!< ���ݎ���
	UINT8 m_cLastTime[8];				//!< �Ō�̎���
	UINT8 m_cDirty;						//!< �`��t���O drawing;
	UINT8 m_cCharaters;					//!< ������
	RGB32 m_pal32[4];					//!< �p���b�g
	RGB16 m_pal16[4];					//!< �p���b�g
	UINT32 m_pal8[4][16];				//!< �p���b�g �p�^�[��
	UINT8 m_buffer[(DCLOCK_HEIGHT * DCLOCK_YALIGN) + 4];	/*!< �o�b�t�@ */

private:
	void SetPalette8();
	void SetPalette16();
	static UINT8 CountPos(UINT nCount);
	void Draw8(void* lpBuffer, int nYAlign) const;
	void Draw16(void* lpBuffer, int nYAlign) const;
	void Draw24(void* lpBuffer, int nYAlign) const;
	void Draw32(void* lpBuffer, int nYAlign) const;
};

/**
 * �C���X�^���X�𓾂�
 * @return �C���X�^���X
 */
inline DispClock* DispClock::GetInstance()
{
	return &sm_instance;
}

/**
 * �p���b�g�𓾂�
 * @return �p���b�g
 */
inline const RGB32* DispClock::GetPalettes() const
{
	return m_pal32;
}
