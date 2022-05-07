/**
 * @file	commng.cpp
 * @brief	COM �}�l�[�W���̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "commng.h"
#include "np2.h"
#include "commng/cmmidi.h"
#include "commng/cmnull.h"
#include "commng/cmpara.h"
#include "commng/cmserial.h"
#include "generic/cmjasts.h"

/**
 * ������
 */
void commng_initialize(void)
{
	cmmidi_initailize();
}

/**
 * �쐬
 * @param[in] nDevice �f�o�C�X
 * @return �n���h��
 */
COMMNG commng_create(UINT nDevice)
{
	COMMNG ret = NULL;

	COMCFG* pComCfg = NULL;
	switch (nDevice)
	{
		case COMCREATE_SERIAL:
			pComCfg = &np2oscfg.com1;
			break;

		case COMCREATE_PC9861K1:
			pComCfg = &np2oscfg.com2;
			break;

		case COMCREATE_PC9861K2:
			pComCfg = &np2oscfg.com3;
			break;

		case COMCREATE_PRINTER:
			if (np2oscfg.jastsnd)
			{
				ret = cmjasts_create();
			}
			break;

		case COMCREATE_MPU98II:
			pComCfg = &np2oscfg.mpu;
			break;

		default:
			break;
	}

	if (pComCfg)
	{
		if ((pComCfg->port >= COMPORT_COM1) && (pComCfg->port <= COMPORT_COM4))
		{
			ret = CComSerial::CreateInstance(pComCfg->port - COMPORT_COM1 + 1, pComCfg->param, pComCfg->speed);
		}
		else if (pComCfg->port == COMPORT_MIDI)
		{
			ret = CComMidi::CreateInstance(pComCfg->mout, pComCfg->min, pComCfg->mdl);
			if (ret)
			{
				ret->msg(ret, COMMSG_MIMPIDEFFILE, (INTPTR)pComCfg->def);
				ret->msg(ret, COMMSG_MIMPIDEFEN, (INTPTR)pComCfg->def_en);
			}
		}
	}

	if (ret == NULL)
	{
		ret = new CComNull;
	}
	return ret;
}

/**
 * �j��
 * @param[in] hdl �n���h��
 */
void commng_destroy(COMMNG hdl)
{
	if (hdl)
	{
		hdl->release(hdl);
	}
}
