///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 Edouard Griffiths, F4EXB                                   //
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

#ifndef PLUGINS_SAMPLESOURCE_SDRDAEMONSOURCE_SDRDAEMONSOURCEUDPHANDLER_H_
#define PLUGINS_SAMPLESOURCE_SDRDAEMONSOURCE_SDRDAEMONSOURCEUDPHANDLER_H_

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMutex>
#include <QElapsedTimer>

#include "sdrdaemonsourcebuffer.h"

#define SDRDAEMONSOURCE_THROTTLE_MS 50

class SampleSinkFifo;
class MessageQueue;
class QTimer;
class DeviceSourceAPI;

class SDRdaemonSourceUDPHandler : public QObject
{
	Q_OBJECT
public:
	SDRdaemonSourceUDPHandler(SampleSinkFifo* sampleFifo, MessageQueue *outputMessageQueueToGUI, DeviceSourceAPI *deviceAPI);
	~SDRdaemonSourceUDPHandler();
	void connectTimer(const QTimer* timer);
	void start();
	void stop();
	void configureUDPLink(const QString& address, quint16 port);
	void getRemoteAddress(QString& s) const { s = m_remoteAddress.toString(); }
    int getNbOriginalBlocks() const { return SDRdaemonSourceBuffer::m_nbOriginalBlocks; }
public slots:
	void dataReadyRead();

private:
	DeviceSourceAPI *m_deviceAPI;
	SDRdaemonSourceBuffer m_sdrDaemonBuffer;
	QUdpSocket *m_dataSocket;
	QHostAddress m_dataAddress;
	QHostAddress m_remoteAddress;
	quint16 m_dataPort;
	bool m_dataConnected;
	char *m_udpBuf;
	qint64 m_udpReadBytes;
	SampleSinkFifo *m_sampleFifo;
	uint32_t m_samplerate;
	uint32_t m_centerFrequency;
	uint32_t m_tv_sec;
	uint32_t m_tv_usec;
	MessageQueue *m_outputMessageQueueToGUI;
	uint32_t m_tickCount;
	std::size_t m_samplesCount;
    QTimer *m_timer;

	QElapsedTimer m_elapsedTimer;
	int m_throttlems;
    uint32_t m_readLengthSamples;
    uint32_t m_readLength;
    bool m_throttleToggle;
    uint32_t m_rateDivider;
    bool m_autoCorrBuffer;

	void processData();

private slots:
	void tick();
};



#endif /* PLUGINS_SAMPLESOURCE_SDRDAEMONSOURCE_SDRDAEMONSOURCEUDPHANDLER_H_ */
