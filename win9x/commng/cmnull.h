/**
 * @file	cmnull.h
 * @brief	commng NULL �f�o�C�X �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "cmbase.h"

/**
 * @brief commng NULL �f�o�C�X �N���X
 */
class CComNull : public CComBase
{
public:
	CComNull();

protected:
	virtual UINT Read(UINT8* pData);
	virtual UINT Write(UINT8 cData);
	virtual UINT8 GetStat();
	virtual INTPTR Message(UINT nMessage, INTPTR nParam);
};
