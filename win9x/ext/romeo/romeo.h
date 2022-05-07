/**
 * @file	romeo.h
 * @brief	ROMEO �p�� PCI ��`�ł�
 */

#pragma once

#define	ROMEO_VENDORID		0x6809		/*!< �x���_�[ ID */
#define	ROMEO_DEVICEID		0x2151		/*!< �f�o�C�X ID */
#define	ROMEO_DEVICEID2		0x8121		/*!< for Developer version */

/**
 * PCIDEBUG ���U���g �R�[�h
 */
enum
{
	PCIERR_SUCCESS			= 0x00,		/*!< ���� */
	PCIERR_INVALIDCLASS		= 0x83,		/*!< �s���ȃf�o�C�X �N���X */
	PCIERR_DEVNOTFOUND		= 0x86		/*!< �f�o�C�X��������Ȃ� */
};

/**
 * �R���t�B�O���[�V���� ���W�X�^
 */
enum
{
	ROMEO_DEVICE_VENDOR		= 0x00,		/*!< �x���_/�f�o�C�X ID */
	ROMEO_STATUS_COMMAND	= 0x04,		/*!< �R�}���h/�X�e�[�^�X ���W�X�^ */
	ROMEO_CLASS_REVISON		= 0x08,		/*!< ���r�W���� ID / �N���X �R�[�h */
	ROMEO_HEADTYPE			= 0x0c,		/*!< �L���b�V�� ���C�� �T�C�Y / �}�X�^ ���C�e���V �^�C�} / �w�b�_ �^�C�v */
	ROMEO_BASEADDRESS0		= 0x10,		/*!< �x�[�X �A�h���X 0 */
	ROMEO_BASEADDRESS1		= 0x14,		/*!< �x�[�X �A�h���X 1 */
	ROMEO_SUB_DEVICE_VENDOR	= 0x2c,		/*!< �T�u �V�X�e�� �x���_ID */
	ROMEO_PCIINTERRUPT		= 0x3c		/*!< �C���^���v�g ���C�� / �C���^���v�g �s�� / �ŏ��O�����g / �ő僌�C�e���V */
};

/**
 * �A�h���X
 */
enum
{
	ROMEO_YM2151ADDR		= 0x0000,	/*!< YM2151 �A�h���X */
	ROMEO_YM2151DATA		= 0x0004,	/*!< YM2151 �f�[�^ */
	ROMEO_CMDQUEUE			= 0x0018,	/*!< �R�}���h �L���[ */
	ROMEO_YM2151CTRL		= 0x001c,	/*!< YM2151 �R���g���[�� */
	ROMEO_YMF288ADDR1		= 0x0100,	/*!< YMF288 �A�h���X */
	ROMEO_YMF288DATA1		= 0x0104,	/*!< YMF288 �f�[�^ */
	ROMEO_YMF288ADDR2		= 0x0108,	/*!< YMF288 �g���A�h���X */
	ROMEO_YMF288DATA2		= 0x010c,	/*!< YMF288 �g���f�[�^ */
	ROMEO_YMF288CTRL		= 0x011c	/*!< YMF288 �R���g���[�� */
};
