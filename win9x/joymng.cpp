/**
 *	@file	joymng.cpp
 *	@brief	ジョイパッド入力の動作の定義を行います
 */

#include "compiler.h"
#include "joymng.h"
#include "np2.h"

#pragma comment(lib, "winmm.lib")

/**
 * ビット定義
 */
enum
{
	JOY_LEFT_BIT	= 0x04,		//!< 左
	JOY_RIGHT_BIT	= 0x08,		//!< 右
	JOY_UP_BIT		= 0x01,		//!< 上
	JOY_DOWN_BIT	= 0x02,		//!< 下
	JOY_BTN1_BIT	= 0x10,		//!< ボタン1
	JOY_BTN2_BIT	= 0x20		//!< ボタン2
};

static bool s_bEnabled = false;				//!< 有効フラグ
static UINT8 s_cJoyFlag = 0;				//!< ステータス
static UINT8 s_sJoyPad1ButtonBit[4];		//!< パッドに割り当てたボタン ビット

/**
 * 初期化
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
 * 有効?
 * @return 有効フラグ
 */
bool joymng_isEnabled()
{
	return s_bEnabled;
}

/**
 * 同期
 */
void joymng_sync()
{
	s_cJoyFlag = 0;
}

/**
 * ステータスを得る
 * @return ステータス
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
