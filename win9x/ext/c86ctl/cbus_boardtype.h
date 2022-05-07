/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#ifndef CBUS_BOARDTYPE_H__
#define CBUS_BOARDTYPE_H__

// CBUS�{�[�h��ʒ�`
//   ���16bit: �A�h���X��ʂȂǃo���G�[�V����
//   ����16bit: �{�[�h��� (��������4bit�͓��ڃ`�b�v�ɂ���G�c�ȕ���)
typedef enum {
	// ������������Ȃ� or �����h�����Ă��Ȃ�
	CBUS_BOARD_UNKNOWN					= 0x0,
	
	// PSG�n =========================================================
	// (���Ή�) NEC PC-9801-14
	//   �A�h���X�f�R�[�_�F 14bit
	CBUS_BOARD_14						= 0x00001,

	// (���Ή�) �V�X�e���T�R�� AMD-98
	//   �A�h���X�f�R�[�_: 16bit(������)
	//   SOUNDID(0xA460) : ����
	//   IDADDR: 0x00f0,  00xx x000 1101 xaaa (0x00d0~0x0df)
	CBUS_BOARD_AMD98					= 0x00011,

	// YM2203(OPN)�n =================================================
	// NEC PC-9801-26, 26K
	//   �A�h���X�f�R�[�_�F 12bit
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0188h/018Ah
	CBUS_BOARD_26						= 0x00002,

	// (���e�X�g) SNE Sound Orchestra : YM2203, YM3812
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR(OPN)   : 0188h/018Ah or 0088h/008Ah
	//   IOADDR(YM3812): 018Ch/018Eh or 008Ch/008Eh
	CBUS_BOARD_SOUND_ORCHESTRA			= 0x00012,
	CBUS_BOARD_SOUND_ORCHESTRA_0188H	= 0x00012,
	CBUS_BOARD_SOUND_ORCHESTRA_0088H	= 0x10012,

	// (���e�X�g) SNE Sound Orchestra L : YM2203, YM3812
	//   �A�h���X�f�R�[�_�F 12bit
	//   SOUNDID(0xA460) : ����
	//   IOADDR(OPN)  : 0188h/018Ah or 0088h/008Ah
	//   IOADDR(Y8950): 018Ch/018Eh or 008Ch/008Eh
	CBUS_BOARD_SOUND_ORCHESTRA_L		= 0x00022,
	CBUS_BOARD_SOUND_ORCHESTRA_L_0188H	= 0x00022,
	CBUS_BOARD_SOUND_ORCHESTRA_L_0088H	= 0x10022,
	
	// (���e�X�g) SNE Sound Orchestra V : YM2203, Y8950(w/ADPCM-RAM)
	//   �A�h���X�f�R�[�_�F 12bit
	//   SOUNDID(0xA460) : ����
	//   IOADDR(OPN)  : 0188h/018Ah or 0088h/008Ah
	//   IOADDR(Y8950): 018Ch/018Eh or 008Ch/008Eh
	CBUS_BOARD_SOUND_ORCHESTRA_V		= 0x00032,
	CBUS_BOARD_SOUND_ORCHESTRA_V_0188H	= 0x00032,
	CBUS_BOARD_SOUND_ORCHESTRA_V_0088H	= 0x10032,
	
	// (���e�X�g) SNE Sound Orchestra VS : YM2203, Y8950(w/ADPCM-RAM)
	//   �A�h���X�f�R�[�_�F 12bit
	//   SOUNDID(0xA460) : ����
	//   IOADDR(OPN)  : 0188h/018Ah or 0088h/008Ah
	//   IOADDR(Y8950): 018Ch/018Eh or 008Ch/008Eh
	CBUS_BOARD_SOUND_ORCHESTRA_VS		= 0x00042,
	CBUS_BOARD_SOUND_ORCHESTRA_VS_0188H	= 0x00042,
	CBUS_BOARD_SOUND_ORCHESTRA_VS_0088H	= 0x10042,
	
	// (���e�X�g) SNE Sound Orchestra LS : YM2203, Y8950
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR(OPN): 0188h/018Ah or 0088h/008Ah
	//   IOADDR(Y8950): 018Ch/018Eh or 008Ch/008Eh
	CBUS_BOARD_SOUND_ORCHESTRA_LS		= 0x00052,
	CBUS_BOARD_SOUND_ORCHESTRA_LS_0188H	= 0x00052,
	CBUS_BOARD_SOUND_ORCHESTRA_LS_0088H	= 0x10052,
	
	// (���e�X�g) SNE Sound Orchestra MATE : YM2203, Y8950
	CBUS_BOARD_SOUND_ORCHESTRA_MATE		= 0x00062,
	// (���e�X�g) SNE Multimedia Orchestra : YM2203, YM262M
	CBUS_BOARD_MULTIMEDIA_ORCHESTRA		= 0x00072,

	// (���e�X�g) SNE Littele Orchestra : YM2203
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_LITTLE_ORCHESTRA			= 0x00082,
	CBUS_BOARD_LITTLE_ORCHESTRA_0188H	= 0x00082,
	CBUS_BOARD_LITTLE_ORCHESTRA_0088H	= 0x10082,

	// (���e�X�g) SNE Littele Orchestra L : YM2203
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_LITTLE_ORCHESTRA_L		= 0x00092,
	CBUS_BOARD_LITTLE_ORCHESTRA_L_0188H	= 0x00092,
	CBUS_BOARD_LITTLE_ORCHESTRA_L_0088H	= 0x01092,
	
	// (���e�X�g) SNE Littele Orchestra RS : YM2203
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_LITTLE_ORCHESTRA_RS		= 0x000a2,
	CBUS_BOARD_LITTLE_ORCHESTRA_RS_0188H= 0x000a2,
	CBUS_BOARD_LITTLE_ORCHESTRA_RS_0088H= 0x100a2,
	
	// (���e�X�g) SNE Littele Orchestra LS : YM2203
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_LITTLE_ORCHESTRA_LS		= 0x000b2,
	CBUS_BOARD_LITTLE_ORCHESTRA_LS_0188H= 0x000b2,
	CBUS_BOARD_LITTLE_ORCHESTRA_LS_0088H= 0x100b2,
	
	// (���e�X�g) SNE Littele Orchestra SS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_SS		= 0x000c2,
	// (���e�X�g) SNE Littele Orchestra MATE : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_MATE	= 0x000d2,
	// (���e�X�g) SNE Littele Orchestra FELLOW : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW	= 0x000e2,
	// (���e�X�g) SNE JOY-2 : YM2203
	CBUS_BOARD_JOY2						= 0x000f2,
	// (���e�X�g) SNE SOUND GRANPRI : YM2203
	CBUS_BOARD_SOUND_GRANPRI			= 0x00102,
	// (���e�X�g) �����j�[�Y TN-F3FM : YM2203C
	CBUS_BOARD_TN_F3FM					= 0x00112,


	// YM2608(OPNA)�n =================================================
	// NEC PC-9801-73
	//   �A�h���X�f�R�[�_�F �s��(16�`20bit)
	//   SOUNDID(0xA460) : 0x20 / 0x30 (86�݊�)
	//   IOADDR: 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh
	CBUS_BOARD_73						= 0x00003,
	CBUS_BOARD_73_0188H					= 0x00003,
	CBUS_BOARD_73_0288H					= 0x10003,

	// NEC PC-9801-86
	//   �A�h���X�f�R�[�_�F �s��(16�`20bit)
	//   SOUNDID(0xA460) : 0x40 / 0x50 (86�݊�)
	//   IOADDR: 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh
	CBUS_BOARD_86						= 0x00023,
	CBUS_BOARD_86_0188H					= 0x00023,
	CBUS_BOARD_86_0288H					= 0x10023,
	
	// (���e�X�g) SIS �A�~���[�Y�����g�T�E���h�{�[�hASB-01 : YM2608
	//   �A�h���X�f�R�[�_: �s��
	//   0xA460 SOUNDID : ����
    //   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh or 0388h/038Ah/038Ch/038Eh
	CBUS_BOARD_ASB01					= 0x00043,
	CBUS_BOARD_ASB01_0188H				= 0x00043,
	CBUS_BOARD_ASB01_0088H				= 0x10043,
	CBUS_BOARD_ASB01_0288H				= 0x20043,
	CBUS_BOARD_ASB01_0388H				= 0x30043,

	// (���e�X�g) �A�C�h���W���p�� SpeakBoard : YM2608(w/ADPCM-RAM)
	//   �A�h���X�f�R�[�_: 12bit
	//   IOADDR=0188h/018Ah/018Ch/018Eh or 0088h/008Ah/008Ch/008Eh
	CBUS_BOARD_SPEAKBOARD				= 0x00053,
	CBUS_BOARD_SPEAKBOARD_188H  		= 0x00053,
	CBUS_BOARD_SPEAKBOARD_088H  		= 0x10053,

	// (���e�X�g) �R���s���[�^�e�N�j�J SPB-98 : YM2608, YMF278
	CBUS_BOARD_SOUNDPLAYER98			= 0x00063,

	// (���Ή�) second-bus86 : YM2608, YMF278B-S
	CBUS_BOARD_SECONDBUS86				= 0x00073,

	// (���Ή�) sound-edge : YAMAHA Sound Edge SW20-98 : YM2608B, YMF278B
	CBUS_BOARD_SOUNDEDGE				= 0x00083,

	// (���Ή�) win-duo : YM2608
	CBUS_BOARD_WINDUO					= 0x00093,

	// (���e�X�g) MAD FACTORY ���� : YM2608(w/ADPCM-RAM), YM3438
	//   �A�h���X�f�R�[�_�F �s��
	//   SOUNDID(0xA460) : ����
	//   IOADDR: 0088h/008Ah/008Ch/008Eh or 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh or 0388h/038Ah/038Ch/038Eh
	//           0488h/048Ah/048Ch/048Eh or 0588h/058Ah/058Ch/058Eh or 0688h/068Ah/068Ch/068Eh or 0788h/078Ah/078Ch/078Eh
	CBUS_BOARD_OTOMI					= 0x000a3,
	CBUS_BOARD_OTOMI_188H				= 0x000a3,
	CBUS_BOARD_OTOMI_088H				= 0x100a3,
	CBUS_BOARD_OTOMI_288H				= 0x200a3,
	CBUS_BOARD_OTOMI_388H				= 0x300a3,

	// Q-Vision WaveMaster(86�݊�) : YM2608, CS4231
	//   �A�h���X�f�R�[�_�F 16bit�ȏ�
	//   SOUNDID(0xA460) : 0x40 / 0x50 (86�݊�)
	//   IOADDR: 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh
	CBUS_BOARD_WAVEMASTER				= 0x000b3,
	CBUS_BOARD_WAVEMASTER_0188H			= 0x000b3,
	CBUS_BOARD_WAVEMASTER_0288H			= 0x100b3,
	
	// Q-Vision WaveSMIT(86�݊�) : YMF288-S, CS4231
	//   �A�h���X�f�R�[�_�F 16bit�ȏ�
	//   SOUNDID(0xA460) : 0x40 / 0x50 (86�݊�)
	//   IOADDR: 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh
	CBUS_BOARD_WAVESMIT					= 0x000c3,
	CBUS_BOARD_WAVESMIT_0188H			= 0x000c3,
	CBUS_BOARD_WAVESMIT_0288H			= 0x100c3,
	
	// Q-Vision WaveStar(86�݊�) : YMF288-S, CS4231
	//   �A�h���X�f�R�[�_�F 16bit�ȏ�
	//   SOUNDID(0xA460) : 0x40 / 0x50 (86�݊�)
	//   IOADDR: 0188h/018Ah/018Ch/018Eh or 0288h/028Ah/028Ch/028Eh
	CBUS_BOARD_WAVESTAR					= 0x000d3,
	CBUS_BOARD_WAVESTAR_0188H			= 0x000d3,
	CBUS_BOARD_WAVESTAR_0288H			= 0x100d3,
	
	// (���Ή�) Buffalo WSN-A4F/A2F : YMF288-S
	//   �A�h���X�f�R�[�_: 16bit�ȏ�
	//   SOUNDID : ����
	//   IOADDR : 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_WSN_A4F					= 0x000e3,
	
	// (���Ή�) SXM-F : YMF288-M
	//   �A�h���X�f�R�[�_: �s��
	//   SOUNDID : ����
	//   IOADDR : 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_SXM_F					= 0x000f3,
	
	// (���Ή�) SRN-F : YMF288-M
	//   �A�h���X�f�R�[�_: 16bit�ȏ�
	//   SOUNDID : ����
	//   IOADDR : 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_SRN_F					= 0x00103,


	// YM262(OPL3)�n =================================================
	// (���Ή�) sound-blaster 16 (CT2720) : YMF262-F
	//   �A�h���X�f�R�[�_ : �s��(16bit�ȉ�)
	//   IOADDR(OPL3&PCM?): 20Dxh�`? (x=2/4/6/8/A/C/E)
	CBUS_BOARD_SB16						= 0x00004,
	
	// (���Ή�) sound-blaster 16 with YM2203 (CT2720) : YMF262-F, YM2203
	//   �A�h���X�f�R�[�_ : �s��(16bit�ȉ�)
	//   IOADDR(OPN) : 0088h/008Ah or 0188h/018Ah
	//   IOADDR(OPL3&PCM?): 20Dxh�`? (x=2/4/6/8/A/C/E)
	CBUS_BOARD_SB16_2203				= 0x00014,
	CBUS_BOARD_SB16_2203_0188H			= 0x00014,
	CBUS_BOARD_SB16_2203_0088H			= 0x10014,
	
	// (���Ή�) sound-blaster 16Value (CT3720) : YMF262-F
	//   �A�h���X�f�R�[�_ : �s��(16bit�ȉ�)
	CBUS_BOARD_SB16VALUE				= 0x00024,
	
	// (���Ή�) canopus PowerWindow T64S : YMF262-M
	CBUS_BOARD_POWERWINDOW_T64S			= 0x00034,
	// (���Ή�) EPSON PCSB2 : YMF262-M
	CBUS_BOARD_PCSB2					= 0x00044,
	// (���Ή�) �R���s���[�^�e�N�j�J : YMF262-M
	CBUS_BOARD_WGS98S					= 0x00054,
	// (���Ή�) buffalo SRB-G : YMF264-F, YMZ263B-F
	CBUS_BOARD_SRB_G					= 0x00064,
	// (���Ή�) SNE MIDI ORCHESTRA MIDI-3 : YM264F
	CBUS_BOARD_MIDI_ORCHESTRA_MIDI3		= 0x00074,


	// YM289�n =================================================
	// (���Ή�) SoundBlaster AWE32 (CT3610) : YMF289B-S
	CBUS_BOARD_SB_AWE32					= 0x00005,
	
	// YM297�n =================================================
	// NEC PC-9801-118 : YMF297-F
	//   �A�h���X�f�R�[�_�F �s��(16�`20bit)
	//   SOUNDID(0xA460) : 0x80
	//   IOADDR: 0188h/018Ah/018Ch/018Eh
	CBUS_BOARD_118						= 0x00006

} CBUS_BOARD_TYPE;


#endif
