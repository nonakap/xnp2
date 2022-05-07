/**
 * @file	vstbuffer.h
 * @brief	VST �o�b�t�@ �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief VST �o�b�t�@ �N���X
 */
class CVstBuffer
{
public:
	CVstBuffer();
	CVstBuffer(UINT nChannels, UINT nSamples);
	~CVstBuffer();
	void Alloc(UINT nChannels, UINT nSamples);
	void Delloc();
	void ZeroFill();
	float** GetBuffer();
	void GetShort(short* lpBuffer) const;

private:
	UINT m_nChannels;		/*!< �`�����l���� */
	UINT m_nSamples;		/*!< �T���v���� */
	float** m_pBuffers;		/*!< �o�b�t�@ */
};

/**
 * �o�b�t�@�𓾂�
 * @return �o�b�t�@
 */
inline float** CVstBuffer::GetBuffer()
{
	return m_pBuffers;
}
