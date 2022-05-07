/**
 *	@file	joymng.h
 *	@brief	ジョイパッド入力の宣言およびインターフェイスの定義をします
 */

#pragma once

#ifdef __cplusplus
extern "C"
{

void joymng_initialize();
bool joymng_isEnabled();
void joymng_sync();

#endif

REG8 joymng_getstat(void);

#ifdef __cplusplus
}
#endif
