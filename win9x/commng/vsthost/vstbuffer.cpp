/**
 * @file	vstbuffer.cpp
 * @brief	VST �o�b�t�@ �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "vstbuffer.h"

/**
 * �R���X�g���N�^
 */
CVstBuffer::CVstBuffer()
	: m_nChannels(0)
	, m_nSamples(0)
	, m_pBuffers(NULL)
{
}

/**
 * �R���X�g���N�^
 * @param[in] nChannels �`���l����
 * @param[in] nSamples �T���v����
 */
CVstBuffer::CVstBuffer(UINT nChannels, UINT nSamples)
	: m_nChannels(0)
	, m_nSamples(0)
	, m_pBuffers(NULL)
{
	Alloc(nChannels, nSamples);
	ZeroFill();
}

/**
 * �f�X�g���N�^
 */
CVstBuffer::~CVstBuffer()
{
	Delloc();
}

/**
 * �R���X�g���N�^
 * @param[in] nChannels �`���l����
 * @param[in] nSamples �T���v����
 */
void CVstBuffer::Alloc(UINT nChannels, UINT nSamples)
{
	if ((m_nChannels == nChannels) && (m_nSamples == nSamples))
	{
		return;
	}

	Delloc();
	if ((nChannels == 0) || (nSamples == 0))
	{
		return;
	}

	m_nChannels = nChannels;
	m_nSamples = nSamples;
	m_pBuffers = new float* [nChannels];
	for (UINT i = 0; i < nChannels; i++)
	{
		m_pBuffers[i] = new float [nSamples];
	}
	ZeroFill();
}

/**
 * �j��
 */
void CVstBuffer::Delloc()
{
	for (UINT i = 0; i < m_nChannels; i++)
	{
		delete[] m_pBuffers[i];
	}
	delete[] m_pBuffers;

	m_nChannels = 0;
	m_nSamples = 0;
	m_pBuffers = NULL;
}

/**
 * Zerofill
 */
void CVstBuffer::ZeroFill()
{
	for (UINT i = 0; i < m_nChannels; i++)
	{
		memset(m_pBuffers[i], 0, sizeof(float) * m_nSamples);
	}
}

/**
 * �T���v���𓾂�
 * @param[out] lpBuffer �o�b�t�@
 */
void CVstBuffer::GetShort(short* lpBuffer) const
{
	for (UINT i = 0; i < m_nChannels; i++)
	{
		const float* p = m_pBuffers[i];
		short* q = lpBuffer + i;
		for (UINT j = 0; j < m_nSamples; j++)
		{
			float s = p[j];
			if (s < -1.0f)
			{
				s = -1.0f;
			}
			else if (s > 1.0f)
			{
				s = 1.0f;
			}
			q[j * m_nChannels] = static_cast<short>((s * 32767.5f) - 0.5f);
		}
	}
}
