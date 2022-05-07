/**
 * @file	cmbase.h
 * @brief	commng ���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "commng.h"

/**
 * @brief commng ���N���X
 */
class CComBase : public _commng
{
protected:
	CComBase(UINT nConnect);
	virtual ~CComBase();

	/**
	 * Read
	 * @param[out] pData
	 * @return result
	 */
	virtual UINT Read(UINT8* pData) = 0;

	/**
	 * Write
	 * @param[in] cData
	 * @return result
	 */
	virtual UINT Write(UINT8 cData) = 0;

	/**
	 * �X�e�[�^�X�𓾂�
	 * @return �X�e�[�^�X
	 */
	virtual UINT8 GetStat() = 0;

	/**
	 * ���b�Z�[�W
	 * @param[in] nMessage ���b�Z�[�W
	 * @param[in] nParam �p�����^
	 * @return ���U���g �R�[�h
	 */
	virtual INTPTR Message(UINT nMessage, INTPTR nParam) = 0;

private:
	static UINT cRead(COMMNG cm, UINT8* pData);
	static UINT cWrite(COMMNG cm, UINT8 cData);
	static UINT8 cGetStat(COMMNG cm);
	static INTPTR cMessage(COMMNG cm, UINT nMessage, INTPTR nParam);
	static void cRelease(COMMNG cm);
};
