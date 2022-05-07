/**
 *	@file	joymng.cpp
 *	@brief	�W���C�p�b�h���͂̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "joymng.h"
#include "np2.h"

#pragma comment(lib, "winmm.lib")

/**
 * �r�b�g��`
 */
enum
{
	JOY_LEFT_BIT	= 0x04,		//!< ��
	JOY_RIGHT_BIT	= 0x08,		//!< �E
	JOY_UP_BIT		= 0x01,		//!< ��
	JOY_DOWN_BIT	= 0x02,		//!< ��
	JOY_BTN1_BIT	= 0x10,		//!< �{�^��1
	JOY_BTN2_BIT	= 0x20		//!< �{�^��2
};

static bool s_bEnabled = false;				//!< �L���t���O
static UINT8 s_cJoyFlag = 0;				//!< �X�e�[�^�X
static UINT8 s_sJoyPad1ButtonBit[4];		//!< �p�b�h�Ɋ��蓖�Ă��{�^�� �r�b�g

/**
 * ������
 */
void joymng_initialize()
{
	JOYINFO ji;
	s_bEnabled = ((joyGetNumDevs() != 0) && (joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR));
	for (int i = 0; i < 4; i++)
	{
		s_sJoyPad1ButtonBit[i] = 0xff ^ ((np2oscfg.JOY1BTN[i] & 3) << ((np2oscfg.JOY1BTN[i] & 4) ? 4 : 6));
	}
}

/**
 * �L��?
 * @return �L���t���O
 */
bool joymng_isEnabled()
{
	return s_bEnabled;
}

/**
 * ����
 */
void joymng_sync()
{
	s_cJoyFlag = 0;
}

/**
 * �X�e�[�^�X�𓾂�
 * @return �X�e�[�^�X
 */
REG8 joymng_getstat(void)
{
	if (s_cJoyFlag == 0)
	{
		UINT8 cJoyFlag = 0xff;
		JOYINFO ji;
		if ((np2oscfg.JOYPAD1) && (::joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR))
		{
			if (ji.wXpos < 0x4000U)
			{
				cJoyFlag &= ~JOY_LEFT_BIT;
			}
			else if (ji.wXpos > 0xc000U)
			{
				cJoyFlag &= ~JOY_RIGHT_BIT;
			}
			if (ji.wYpos < 0x4000U)
			{
				cJoyFlag &= ~JOY_UP_BIT;
			}
			else if (ji.wYpos > 0xc000U)
			{
				cJoyFlag &= ~JOY_DOWN_BIT;
			}
			if (ji.wButtons & JOY_BUTTON1)
			{
				cJoyFlag &= s_sJoyPad1ButtonBit[0];
			}
			if (ji.wButtons & JOY_BUTTON2)
			{
				cJoyFlag &= s_sJoyPad1ButtonBit[1];
			}
			if (ji.wButtons & JOY_BUTTON3)
			{
				cJoyFlag &= s_sJoyPad1ButtonBit[2];
			}
			if (ji.wButtons & JOY_BUTTON4)
			{
				cJoyFlag &= s_sJoyPad1ButtonBit[3];
			}
		}
		s_cJoyFlag = cJoyFlag;
	}
	return s_cJoyFlag;
}
