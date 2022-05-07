/**
 * @file	cmserial.h
 * @brief	�V���A�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "cmbase.h"

extern const UINT32 cmserial_speed[10];

/**
 * @brief commng �V���A�� �f�o�C�X �N���X
 */
class CComSerial : public CComBase
{
public:
	static CComSerial* CreateInstance(UINT nPort, UINT8 cParam, UINT32 nSpeed);

protected:
	CComSerial();
	virtual ~CComSerial();
	virtual UINT Read(UINT8* pData);
	virtual UINT Write(UINT8 cData);
	virtual UINT8 GetStat();
	virtual INTPTR Message(UINT nMessage, INTPTR nParam);

private:
	HANDLE m_hSerial;		/*!< �V���A�� �n���h�� */

	bool Initialize(UINT nPort, UINT8 cParam, UINT32 nSpeed);
};
