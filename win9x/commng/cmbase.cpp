/**
 * @file	cmbase.h
 * @brief	commng ���N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "cmbase.h"

/**
 * �R���X�g���N�^
 * @param[in] nConnect �ڑ��t���O
 */
CComBase::CComBase(UINT nConnect)
{
	this->connect = nConnect;
	this->read = cRead;
	this->write = cWrite;
	this->getstat = cGetStat;
	this->msg = cMessage;
	this->release = cRelease;
}

/**
 * �f�X�g���N�^
 */
CComBase::~CComBase()
{
}

/**
 * Read
 * @param[in] cm COMMNG �C���X�^���X
 * @param[out] pData
 * @return result
 */
UINT CComBase::cRead(COMMNG cm, UINT8* pData)
{
	return static_cast<CComBase*>(cm)->Read(pData);
}

/**
 * Write
 * @param[in] cm COMMNG �C���X�^���X
 * @param[in] cData
 * @return result
 */
UINT CComBase::cWrite(COMMNG cm, UINT8 cData)
{
	return static_cast<CComBase*>(cm)->Write(cData);
}

/**
 * �X�e�[�^�X�𓾂�
 * @param[in] cm COMMNG �C���X�^���X
 * @return �X�e�[�^�X
 */
UINT8 CComBase::cGetStat(COMMNG cm)
{
	return static_cast<CComBase*>(cm)->GetStat();
}

/**
 * ���b�Z�[�W
 * @param[in] cm COMMNG �C���X�^���X
 * @param[in] nMessage ���b�Z�[�W
 * @param[in] nParam �p�����^
 * @return ���U���g �R�[�h
 */
INTPTR CComBase::cMessage(COMMNG cm, UINT nMessage, INTPTR nParam)
{
	return static_cast<CComBase*>(cm)->Message(nMessage, nParam);
}

/**
 * �����[�X
 * @param[in] cm COMMNG �C���X�^���X
 */
void CComBase::cRelease(COMMNG cm)
{
	delete static_cast<CComBase*>(cm);
}
