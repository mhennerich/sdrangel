///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 maintech GmbH, Otto-Hahn-Str. 15, 97204 Hoechberg, Germany //
// written by Christian Daniel                                                   //
// (c) 2014 Modified by John Greb
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include "ssbdemod.h"

#include <dsp/downchannelizer.h>
#include <QTime>
#include <QDebug>
#include <stdio.h>
#include "audio/audiooutput.h"
#include "dsp/dspengine.h"
#include "util/db.h"

MESSAGE_CLASS_DEFINITION(SSBDemod::MsgConfigureSSBDemod, Message)

SSBDemod::SSBDemod(BasebandSampleSink* sampleSink) :
	m_audioBinaual(false),
	m_audioFlipChannels(false),
    m_dsb(false),
    m_audioMute(false),
    m_agc(12000, agcTarget, 1e-2),
    m_agcActive(false),
    m_agcClamping(false),
    m_agcNbSamples(12000),
    m_agcPowerThreshold(1e-2),
    m_agcThresholdGate(0),
    m_audioActive(false),
    m_sampleSink(sampleSink),
    m_audioFifo(24000),
    m_settingsMutex(QMutex::Recursive)
{
	setObjectName("SSBDemod");

	m_Bandwidth = 5000;
	m_LowCutoff = 300;
	m_volume = 2.0;
	m_spanLog2 = 3;
	m_sampleRate = 96000;
	m_frequency = 0;
	m_nco.setFreq(m_frequency, m_sampleRate);
	m_audioSampleRate = DSPEngine::instance()->getAudioSampleRate();

	m_interpolator.create(16, m_sampleRate, 5000);
	m_sampleDistanceRemain = (Real) m_sampleRate / m_audioSampleRate;

	m_audioBuffer.resize(1<<9);
	m_audioBufferFill = 0;
	m_undersampleCount = 0;
	m_sum = 0;

	m_usb = true;
	m_magsq = 0.0f;
	m_magsqSum = 0.0f;
	m_magsqPeak = 0.0f;
	m_magsqCount = 0;

	m_agc.setClampMax(32768.0*32768.0);
	m_agc.setClamping(m_agcClamping);

	SSBFilter = new fftfilt(m_LowCutoff / m_audioSampleRate, m_Bandwidth / m_audioSampleRate, ssbFftLen);
	DSBFilter = new fftfilt((2.0f * m_Bandwidth) / m_audioSampleRate, 2 * ssbFftLen);

	DSPEngine::instance()->addAudioSink(&m_audioFifo);
}

SSBDemod::~SSBDemod()
{
	if (SSBFilter) delete SSBFilter;
	if (DSBFilter) delete DSBFilter;

	DSPEngine::instance()->removeAudioSink(&m_audioFifo);
}

void SSBDemod::configure(MessageQueue* messageQueue,
		Real Bandwidth,
		Real LowCutoff,
		Real volume,
		int spanLog2,
		bool audioBinaural,
		bool audioFlipChannel,
		bool dsb,
		bool audioMute,
		bool agc,
		bool agcClamping,
        int agcTimeLog2,
        int agcPowerThreshold,
        int agcThresholdGate)
{
	Message* cmd = MsgConfigureSSBDemod::create(
	        Bandwidth,
	        LowCutoff,
	        volume,
	        spanLog2,
	        audioBinaural,
	        audioFlipChannel,
	        dsb,
	        audioMute,
	        agc,
	        agcClamping,
	        agcTimeLog2,
	        agcPowerThreshold,
	        agcThresholdGate);
	messageQueue->push(cmd);
}

void SSBDemod::feed(const SampleVector::const_iterator& begin, const SampleVector::const_iterator& end, bool positiveOnly __attribute__((unused)))
{
	Complex ci;
	fftfilt::cmplx *sideband;
	int n_out;

	m_settingsMutex.lock();

	int decim = 1<<(m_spanLog2 - 1);
	unsigned char decim_mask = decim - 1; // counter LSB bit mask for decimation by 2^(m_scaleLog2 - 1)

	for(SampleVector::const_iterator it = begin; it < end; ++it)
	{
		//Complex c(it->real() / 32768.0, it->imag() / 32768.0);
		Complex c(it->real(), it->imag());
		c *= m_nco.nextIQ();

		if(m_interpolator.decimate(&m_sampleDistanceRemain, c, &ci))
		{
			if (m_dsb)
			{
				n_out = DSBFilter->runDSB(ci, &sideband);
			}
			else
			{
				n_out = SSBFilter->runSSB(ci, &sideband, m_usb);
			}

			m_sampleDistanceRemain += (Real)m_sampleRate / m_audioSampleRate;
		}
		else
		{
			n_out = 0;
		}

		for (int i = 0; i < n_out; i++)
		{
			// Downsample by 2^(m_scaleLog2 - 1) for SSB band spectrum display
			// smart decimation with bit gain using float arithmetic (23 bits significand)

			m_sum += sideband[i];

			if (!(m_undersampleCount++ & decim_mask))
			{
				Real avgr = m_sum.real() / decim;
				Real avgi = m_sum.imag() / decim;
				m_magsq = (avgr * avgr + avgi * avgi) / (1<<30);

                m_magsqSum += m_magsq;

                if (m_magsq > m_magsqPeak)
                {
                    m_magsqPeak = m_magsq;
                }

                m_magsqCount++;

				if (!m_dsb & !m_usb)
				{ // invert spectrum for LSB
					m_sampleBuffer.push_back(Sample(avgi, avgr));
				}
				else
				{
					m_sampleBuffer.push_back(Sample(avgr, avgi));
				}

                m_sum.real(0.0);
                m_sum.imag(0.0);
			}

            double agcVal = m_agcActive ? m_agc.feedAndGetValue(sideband[i]) : 10.0; // 10.0 for 3276.8, 1.0 for 327.68
            m_audioActive = agcVal != 0.0;

			if (m_audioMute)
			{
				m_audioBuffer[m_audioBufferFill].r = 0;
				m_audioBuffer[m_audioBufferFill].l = 0;
			}
			else
			{
				if (m_audioBinaual)
				{
					if (m_audioFlipChannels)
					{
						m_audioBuffer[m_audioBufferFill].r = (qint16)(sideband[i].imag() * m_volume * agcVal);
						m_audioBuffer[m_audioBufferFill].l = (qint16)(sideband[i].real() * m_volume * agcVal);
					}
					else
					{
						m_audioBuffer[m_audioBufferFill].r = (qint16)(sideband[i].real() * m_volume * agcVal);
						m_audioBuffer[m_audioBufferFill].l = (qint16)(sideband[i].imag() * m_volume * agcVal);
					}
				}
				else
				{
					Real demod = (sideband[i].real() + sideband[i].imag()) * 0.7;
					qint16 sample = (qint16)(demod * m_volume * agcVal);
					m_audioBuffer[m_audioBufferFill].l = sample;
					m_audioBuffer[m_audioBufferFill].r = sample;
				}
			}

			++m_audioBufferFill;

			if (m_audioBufferFill >= m_audioBuffer.size())
			{
				uint res = m_audioFifo.write((const quint8*)&m_audioBuffer[0], m_audioBufferFill, 1);

				if (res != m_audioBufferFill)
				{
					qDebug("lost %u samples", m_audioBufferFill - res);
				}

				m_audioBufferFill = 0;
			}
		}
	}

	if (m_audioFifo.write((const quint8*)&m_audioBuffer[0], m_audioBufferFill, 0) != m_audioBufferFill)
	{
		qDebug("SSBDemod::feed: lost samples");
	}
	m_audioBufferFill = 0;

	if (m_sampleSink != 0)
	{
		m_sampleSink->feed(m_sampleBuffer.begin(), m_sampleBuffer.end(), !m_dsb);
	}

	m_sampleBuffer.clear();

	m_settingsMutex.unlock();
}

void SSBDemod::start()
{
}

void SSBDemod::stop()
{
}

bool SSBDemod::handleMessage(const Message& cmd)
{
	float band, lowCutoff;

	qDebug() << "SSBDemod::handleMessage";

	if (DownChannelizer::MsgChannelizerNotification::match(cmd))
	{
		DownChannelizer::MsgChannelizerNotification& notif = (DownChannelizer::MsgChannelizerNotification&) cmd;

		m_settingsMutex.lock();

		m_sampleRate = notif.getSampleRate();
		m_nco.setFreq(-notif.getFrequencyOffset(), m_sampleRate);
		m_interpolator.create(16, m_sampleRate, m_Bandwidth);
		m_sampleDistanceRemain = m_sampleRate / m_audioSampleRate;

		m_settingsMutex.unlock();

		qDebug() << "SSBDemod::handleMessage: MsgChannelizerNotification: m_sampleRate: " << m_sampleRate
				<< " frequencyOffset" << notif.getFrequencyOffset();

		return true;
	}
	else if (MsgConfigureSSBDemod::match(cmd))
	{
		MsgConfigureSSBDemod& cfg = (MsgConfigureSSBDemod&) cmd;

		m_settingsMutex.lock();

		band = cfg.getBandwidth();
		lowCutoff = cfg.getLoCutoff();

		if (band < 0) {
			band = -band;
			lowCutoff = -lowCutoff;
			m_usb = false;
		} else
			m_usb = true;

		if (band < 100.0f)
		{
			band = 100.0f;
			lowCutoff = 0;
		}
		m_Bandwidth = band;
		m_LowCutoff = lowCutoff;

		m_interpolator.create(16, m_sampleRate, band * 2.0f);
		SSBFilter->create_filter(m_LowCutoff / (float) m_audioSampleRate, m_Bandwidth / (float) m_audioSampleRate);
		DSBFilter->create_dsb_filter((2.0f * m_Bandwidth) / (float) m_audioSampleRate);

		m_volume = cfg.getVolume();
		//m_volume *= 2.0; // for 327.68
		m_volume /= 4.0; // for 3276.8

		m_spanLog2 = cfg.getSpanLog2();
		m_audioBinaual = cfg.getAudioBinaural();
		m_audioFlipChannels = cfg.getAudioFlipChannels();
		m_dsb = cfg.getDSB();
		m_audioMute = cfg.getAudioMute();
		m_agcActive = cfg.getAGC();

		int agcNbSamples = 48 * (1<<cfg.getAGCTimeLog2());
        m_agc.setThresholdEnable(cfg.getAGCPowerThershold() != -99);
		double agcPowerThreshold = CalcDb::powerFromdB(cfg.getAGCPowerThershold()) * (1<<30);
		int agcThresholdGate = 48 * cfg.getAGCThersholdGate(); // ms
		bool agcClamping = cfg.getAGCClamping();

		if (m_agcNbSamples != agcNbSamples)
		{
		    m_agc.resize(agcNbSamples, agcTarget);
		    m_agc.setStepDownDelay(agcNbSamples);
		    m_agcNbSamples = agcNbSamples;
		}

		if (m_agcPowerThreshold != agcPowerThreshold)
		{
		    m_agc.setThreshold(agcPowerThreshold);
		    m_agcPowerThreshold = agcPowerThreshold;
		}

		if (m_agcThresholdGate != agcThresholdGate)
		{
		    m_agc.setGate(agcThresholdGate);
		    m_agcThresholdGate = agcThresholdGate;
		}

		if (m_agcClamping != agcClamping)
		{
		    m_agc.setClamping(agcClamping);
		    m_agcClamping = agcClamping;
		}

		m_settingsMutex.unlock();

		qDebug() << "SBDemod::handleMessage: MsgConfigureSSBDemod: m_Bandwidth: " << m_Bandwidth
				<< " m_LowCutoff: " << m_LowCutoff
				<< " m_volume: " << m_volume
				<< " m_spanLog2: " << m_spanLog2
				<< " m_audioBinaual: " << m_audioBinaual
				<< " m_audioFlipChannels: " << m_audioFlipChannels
				<< " m_dsb: " << m_dsb
				<< " m_audioMute: " << m_audioMute
				<< " m_agcActive: " << m_agcActive
				<< " m_agcClamping: " << m_agcClamping
				<< " agcNbSamples: " << agcNbSamples
				<< " agcPowerThreshold: " << agcPowerThreshold
				<< " agcThresholdGate: " << agcThresholdGate;

		return true;
	}
	else
	{
		if(m_sampleSink != 0)
		{
		   return m_sampleSink->handleMessage(cmd);
		}
		else
		{
			return false;
		}
	}
}
