/**
 * @file	pccore.h
 * @brief	Interface of the core
 */

#pragma once

#include "nevent.h"

enum {
	PCBASECLOCK25		= 2457600,
	PCBASECLOCK20		= 1996800
};

enum {
	CPUMODE_8MHZ		= 0x20,

	PCMODEL_VF			= 0,
	PCMODEL_VM			= 1,
	PCMODEL_VX			= 2,
	PCMODELMASK			= 0x3f,
	PCMODEL_PC9821		= 0x40,
	PCMODEL_EPSON		= 0x80,

	PCHDD_SASI			= 0x01,
	PCHDD_SCSI			= 0x02,
	PCHDD_IDE			= 0x04,

	PCROM_BIOS			= 0x01,
	PCROM_SOUND			= 0x02,
	PCROM_SASI			= 0x04,
	PCROM_SCSI			= 0x08,
	PCROM_BIOS9821		= 0x10,

	PCCBUS_PC9861K		= 0x0001,
	PCCBUS_MPU98		= 0x0002
};

/**
 * Sound ID
 */
enum tagSoundId
{
	SOUNDID_NONE				= 0,		/*!< No boards */
	SOUNDID_PC_9801_14			= 0x01,		/*!< PC-9801-14 */ 
	SOUNDID_PC_9801_26K			= 0x02,		/*!< PC-9801-26K */ 
	SOUNDID_PC_9801_86			= 0x04,		/*!< PC-9801-86 */ 
	SOUNDID_PC_9801_86_26K		= 0x06,		/*!< PC-9801-86 + 26K */ 
	SOUNDID_PC_9801_118			= 0x08,		/*!< PC-9801-118 */
	SOUNDID_PC_9801_86_ADPCM	= 0x14,		/*!< PC-9801-86 with ADPCM */
	SOUNDID_SPEAKBOARD			= 0x20,		/*!< Speak board */
	SOUNDID_SPARKBOARD			= 0x40,		/*!< Spark board */
	SOUNDID_AMD98				= 0x80,		/*!< AMD-98 */
	SOUNDID_SOUNDORCHESTRA		= 0x32,		/*!< SOUND ORCHESTRA */
	SOUNDID_SOUNDORCHESTRAV		= 0x82,		/*!< SOUND ORCHESTRA-V */

#if defined(SUPPORT_PX)
	SOUNDID_PX1					= 0x30,
	SOUNDID_PX2					= 0x50,
#endif	/* defined(SUPPORT_PX) */
};
typedef enum tagSoundId		SOUNDID;

/**
 * @brief config
 */
struct tagNP2Config
{
	// �G�~�����[�g���ɂ悭�Q�Ƃ����z
	UINT8	uPD72020;
	UINT8	DISPSYNC;
	UINT8	RASTER;
	UINT8	realpal;
	UINT8	LCD_MODE;
	UINT8	skipline;
	UINT16	skiplight;

	UINT8	KEY_MODE;
	UINT8	XSHIFT;
	UINT8	BTN_RAPID;
	UINT8	BTN_MODE;

	UINT8	dipsw[3];
	UINT8	MOUSERAPID;

	UINT8	calendar;
	UINT8	usefd144;
	UINT8	wait[6];

	// ���Z�b�g���Ƃ�����܂�Q�Ƃ���Ȃ��z
	OEMCHAR	model[8];
	UINT	baseclock;
	UINT	multiple;

	UINT8	memsw[8];

	UINT8	ITF_WORK;
	UINT8	EXTMEM;
	UINT8	grcg;
	UINT8	color16;
	UINT32	BG_COLOR;
	UINT32	FG_COLOR;

	UINT32	samplingrate;
	UINT16	delayms;
	UINT8	SOUND_SW;
	UINT8	snd_x;

	UINT8	snd14opt[3];
	UINT8	snd26opt;
	UINT8	snd86opt;
	UINT8	spbopt;
	UINT8	spb_vrc;												// ver0.30
	UINT8	spb_vrl;												// ver0.30
	UINT8	spb_x;													// ver0.30

	UINT8	BEEP_VOL;
	UINT8	vol14[6];
	UINT8	vol_fm;
	UINT8	vol_ssg;
	UINT8	vol_adpcm;
	UINT8	vol_pcm;
	UINT8	vol_rhythm;

	UINT8	mpuenable;
	UINT8	mpuopt;

	UINT8	pc9861enable;
	UINT8	pc9861sw[3];
	UINT8	pc9861jmp[6];

	UINT8	fddequip;
	UINT8	MOTOR;
	UINT8	MOTORVOL;
	UINT8	PROTECTMEM;
	UINT8	hdrvacc;

	OEMCHAR	sasihdd[2][MAX_PATH];									// ver0.74
#if defined(SUPPORT_SCSI)
	OEMCHAR	scsihdd[4][MAX_PATH];									// ver0.74
#endif
	OEMCHAR	fontfile[MAX_PATH];
	OEMCHAR	biospath[MAX_PATH];
	OEMCHAR	hdrvroot[MAX_PATH];
};
typedef struct tagNP2Config  NP2CFG;		/*!< The define of config */

typedef struct {
	UINT32	baseclock;
	UINT	multiple;

	UINT8	cpumode;
	UINT8	model;
	UINT8	hddif;
	UINT8	extmem;
	UINT8	dipsw[3];		// ���Z�b�g����DIPSW
	UINT8	rom;

	SOUNDID sound;
	UINT32	device;

	UINT32	realclock;
	UINT	frames;
} PCCORE;

enum {
	COREEVENT_SHUT		= 0,
	COREEVENT_RESET		= 1,
	COREEVENT_EXIT		= 2
};

typedef struct
{
	UINT8	screenupdate;
	UINT8	screendispflag;
	UINT8	drawframe;
	UINT8	hardwarereset;
} PCSTAT;


#ifdef __cplusplus
extern "C" {
#endif

extern const OEMCHAR np2version[];

extern	NP2CFG	np2cfg;
extern	PCCORE	pccore;
extern	PCSTAT	pcstat;
extern	UINT8	soundrenewal;
extern	UINT	drawcount;

void getbiospath(OEMCHAR *path, const OEMCHAR *fname, UINT maxlen);
void screendisp(NEVENTITEM item);
void screenvsync(NEVENTITEM item);


void pccore_cfgupdate(void);

void pccore_init(void);
void pccore_term(void);
void pccore_reset(void);
void pccore_exec(BOOL draw);

void pccore_postevent(UINT32 event);

#ifdef __cplusplus
}
#endif

