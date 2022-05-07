/**
 * @file	cmpara.h
 * @brief	�p������ �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "cmbase.h"

/**
 * @brief commng �p������ �f�o�C�X �N���X
 */
class CComPara : public CComBase
{
public:
	static CComPara* CreateInstance(UINT nPort);

protected:
	CComPara();
	virtual ~CComPara();
	virtual UINT Read(UINT8* pData);
	virtual UINT Write(UINT8 cData);
	virtual UINT8 GetStat();
	virtual INTPTR Message(UINT nMessage, INTPTR nParam);

private:
	HANDLE m_hParallel;			/*!< �p������ �n���h�� */

	bool Initialize(UINT nPort);
};
