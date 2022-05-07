/**
 * @file	cmnull.h
 * @brief	commng NULL �f�o�C�X �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmnull.h"

/**
 * �R���X�g���N�^
 */
CComNull::CComNull()
	: CComBase(COMCONNECT_OFF)
{
}

/**
 * Read
 * @param[out] pData
 * @return result
 */
UINT CComNull::Read(UINT8* pData)
{
	return 0;
}

/**
 * Write
 * @param[in] cData
 * @return result
 */
UINT CComNull::Write(UINT8 cData)
{
	return 0;
}

/**
 * �X�e�[�^�X�𓾂�
 * @return �X�e�[�^�X
 */
UINT8 CComNull::GetStat()
{
	return 0xf0;
}

/**
 * ���b�Z�[�W
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParam �p�����^
 * @return ���U���g �R�[�h
 */
INTPTR CComNull::Message(UINT nMessage, INTPTR nParam)
{
	return 0;
}
