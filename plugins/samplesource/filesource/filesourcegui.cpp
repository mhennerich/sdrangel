///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
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

#include <QDebug>

#include <QTime>
#include <QDateTime>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_filesourcegui.h"
#include "plugin/pluginapi.h"
#include "gui/colormapper.h"
#include "gui/glspectrum.h"
#include "dsp/dspengine.h"
#include "dsp/dspcommands.h"

#include "mainwindow.h"

#include "filesourcegui.h"
#include <device/devicesourceapi.h>

FileSourceGui::FileSourceGui(DeviceSourceAPI *deviceAPI, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::FileSourceGui),
	m_deviceAPI(deviceAPI),
	m_settings(),
	m_sampleSource(NULL),
	m_acquisition(false),
	m_fileName("..."),
	m_sampleRate(0),
	m_centerFrequency(0),
	m_recordLength(0),
	m_startingTimeStamp(0),
	m_samplesCount(0),
	m_tickCount(0),
	m_enableNavTime(false),
	m_lastEngineState((DSPDeviceSourceEngine::State)-1)
{
	ui->setupUi(this);
	ui->centerFrequency->setColorMapper(ColorMapper(ColorMapper::GrayGold));
	ui->centerFrequency->setValueRange(7, 0, pow(10,7));
	ui->fileNameText->setText(m_fileName);

	connect(&(m_deviceAPI->getMainWindow()->getMasterTimer()), SIGNAL(timeout()), this, SLOT(tick()));
	connect(&m_statusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	m_statusTimer.start(500);

	displaySettings();

	ui->navTimeSlider->setEnabled(false);
	ui->playLoop->setChecked(true); // FIXME: always play in a loop
	ui->playLoop->setEnabled(false);

	m_sampleSource = new FileSourceInput(m_deviceAPI->getMainWindow()->getMasterTimer());
	connect(m_sampleSource->getOutputMessageQueueToGUI(), SIGNAL(messageEnqueued()), this, SLOT(handleSourceMessages()));
	m_deviceAPI->setSource(m_sampleSource);

    connect(m_deviceAPI->getDeviceOutputMessageQueue(), SIGNAL(messageEnqueued()), this, SLOT(handleDSPMessages()), Qt::QueuedConnection);
}

FileSourceGui::~FileSourceGui()
{
	delete ui;
}

void FileSourceGui::destroy()
{
	delete this;
}

void FileSourceGui::setName(const QString& name)
{
	setObjectName(name);
}

QString FileSourceGui::getName() const
{
	return objectName();
}

void FileSourceGui::resetToDefaults()
{
	m_settings.resetToDefaults();
	displaySettings();
	sendSettings();
}

qint64 FileSourceGui::getCenterFrequency() const
{
	return m_centerFrequency;
}

void FileSourceGui::setCenterFrequency(qint64 centerFrequency)
{
	m_centerFrequency = centerFrequency;
	displaySettings();
	sendSettings();
}

QByteArray FileSourceGui::serialize() const
{
	return m_settings.serialize();
}

bool FileSourceGui::deserialize(const QByteArray& data)
{
	if(m_settings.deserialize(data)) {
		displaySettings();
		sendSettings();
		return true;
	} else {
		resetToDefaults();
		return false;
	}
}

void FileSourceGui::handleDSPMessages()
{
    Message* message;

    while ((message = m_deviceAPI->getDeviceOutputMessageQueue()->pop()) != 0)
    {
        qDebug("FileSourceGui::handleDSPMessages: message: %s", message->getIdentifier());

        if (DSPSignalNotification::match(*message))
        {
            DSPSignalNotification* notif = (DSPSignalNotification*) message;
            m_deviceSampleRate = notif->getSampleRate();
            m_deviceCenterFrequency = notif->getCenterFrequency();
            qDebug("FileSourceGui::handleDSPMessages: SampleRate:%d, CenterFrequency:%llu", notif->getSampleRate(), notif->getCenterFrequency());
            updateSampleRateAndFrequency();

            delete message;
        }
    }
}

bool FileSourceGui::handleMessage(const Message& message)
{
	if (FileSourceInput::MsgReportFileSourceAcquisition::match(message))
	{
		m_acquisition = ((FileSourceInput::MsgReportFileSourceAcquisition&)message).getAcquisition();
		updateWithAcquisition();
		return true;
	}
	else if (FileSourceInput::MsgReportFileSourceStreamData::match(message))
	{
		m_sampleRate = ((FileSourceInput::MsgReportFileSourceStreamData&)message).getSampleRate();
		m_centerFrequency = ((FileSourceInput::MsgReportFileSourceStreamData&)message).getCenterFrequency();
		m_startingTimeStamp = ((FileSourceInput::MsgReportFileSourceStreamData&)message).getStartingTimeStamp();
		m_recordLength = ((FileSourceInput::MsgReportFileSourceStreamData&)message).getRecordLength();
		updateWithStreamData();
		return true;
	}
	else if (FileSourceInput::MsgReportFileSourceStreamTiming::match(message))
	{
		m_samplesCount = ((FileSourceInput::MsgReportFileSourceStreamTiming&)message).getSamplesCount();
		updateWithStreamTime();
		return true;
	}
	else
	{
		return false;
	}
}

void FileSourceGui::handleSourceMessages()
{
	Message* message;

	while ((message = m_sampleSource->getOutputMessageQueueToGUI()->pop()) != 0)
	{
		//qDebug("FileSourceGui::handleSourceMessages: message: %s", message->getIdentifier());

		if (handleMessage(*message))
		{
			delete message;
		}
	}
}

void FileSourceGui::updateSampleRateAndFrequency()
{
    m_deviceAPI->getSpectrum()->setSampleRate(m_deviceSampleRate);
    m_deviceAPI->getSpectrum()->setCenterFrequency(m_deviceCenterFrequency);
    ui->deviceRateText->setText(tr("%1k").arg((float)m_deviceSampleRate / 1000));
}

void FileSourceGui::displaySettings()
{
}

void FileSourceGui::sendSettings()
{
}

void FileSourceGui::on_playLoop_toggled(bool checked __attribute__((unused)))
{
	// TODO: do something about it!
}

void FileSourceGui::on_startStop_toggled(bool checked)
{
    if (checked)
    {
        if (m_deviceAPI->initAcquisition())
        {
            m_deviceAPI->startAcquisition();
            DSPEngine::instance()->startAudioOutput();
        }
    }
    else
    {
        m_deviceAPI->stopAcquisition();
        DSPEngine::instance()->stopAudioOutput();
    }
}

void FileSourceGui::updateStatus()
{
    int state = m_deviceAPI->state();

    if(m_lastEngineState != state)
    {
        switch(state)
        {
            case DSPDeviceSourceEngine::StNotStarted:
                ui->startStop->setStyleSheet("QToolButton { background:rgb(79,79,79); }");
                break;
            case DSPDeviceSourceEngine::StIdle:
                ui->startStop->setStyleSheet("QToolButton { background-color : blue; }");
                break;
            case DSPDeviceSourceEngine::StRunning:
                ui->startStop->setStyleSheet("QToolButton { background-color : green; }");
                break;
            case DSPDeviceSourceEngine::StError:
                ui->startStop->setStyleSheet("QToolButton { background-color : red; }");
                QMessageBox::information(this, tr("Message"), m_deviceAPI->errorMessage());
                break;
            default:
                break;
        }

        m_lastEngineState = state;
    }
}

void FileSourceGui::on_play_toggled(bool checked)
{
	FileSourceInput::MsgConfigureFileSourceWork* message = FileSourceInput::MsgConfigureFileSourceWork::create(checked);
	m_sampleSource->getInputMessageQueue()->push(message);
	ui->navTimeSlider->setEnabled(!checked);
	m_enableNavTime = !checked;
}

void FileSourceGui::on_navTimeSlider_valueChanged(int value)
{
	if (m_enableNavTime && ((value >= 0) && (value <= 100)))
	{
		int t_sec = (m_recordLength * value) / 100;
		QTime t(0, 0, 0, 0);
		t = t.addSecs(t_sec);

		FileSourceInput::MsgConfigureFileSourceSeek* message = FileSourceInput::MsgConfigureFileSourceSeek::create(value);
		m_sampleSource->getInputMessageQueue()->push(message);
	}
}

void FileSourceGui::on_showFileDialog_clicked(bool checked __attribute__((unused)))
{
	QString fileName = QFileDialog::getOpenFileName(this,
	    tr("Open I/Q record file"), ".", tr("SDR I/Q Files (*.sdriq)"));

	if (fileName != "")
	{
		m_fileName = fileName;
		ui->fileNameText->setText(m_fileName);
		configureFileName();
	}
}

void FileSourceGui::configureFileName()
{
	qDebug() << "FileSourceGui::configureFileName: " << m_fileName.toStdString().c_str();
	FileSourceInput::MsgConfigureFileSourceName* message = FileSourceInput::MsgConfigureFileSourceName::create(m_fileName);
	m_sampleSource->getInputMessageQueue()->push(message);
}

void FileSourceGui::updateWithAcquisition()
{
	ui->play->setEnabled(m_acquisition);
	ui->play->setChecked(m_acquisition);
	ui->showFileDialog->setEnabled(!m_acquisition);
}

void FileSourceGui::updateWithStreamData()
{
	ui->centerFrequency->setValue(m_centerFrequency/1000);
	ui->sampleRateText->setText(tr("%1k").arg((float)m_sampleRate / 1000));
	ui->play->setEnabled(m_acquisition);
	QTime recordLength(0, 0, 0, 0);
	recordLength = recordLength.addSecs(m_recordLength);
	QString s_time = recordLength.toString("hh:mm:ss");
	ui->recordLengthText->setText(s_time);
	updateWithStreamTime(); // TODO: remove when time data is implemented
}

void FileSourceGui::updateWithStreamTime()
{
	int t_sec = 0;
	int t_msec = 0;

	if (m_sampleRate > 0){
		t_msec = ((m_samplesCount * 1000) / m_sampleRate) % 1000;
		t_sec = m_samplesCount / m_sampleRate;
	}

	QTime t(0, 0, 0, 0);
	t = t.addSecs(t_sec);
	t = t.addMSecs(t_msec);
	QString s_timems = t.toString("hh:mm:ss.zzz");
	QString s_time = t.toString("hh:mm:ss");
	ui->relTimeText->setText(s_timems);

    quint64 startingTimeStampMsec = (quint64) m_startingTimeStamp * 1000LL;
	QDateTime dt = QDateTime::fromMSecsSinceEpoch(startingTimeStampMsec);
    dt = dt.addSecs((quint64) t_sec);
    dt = dt.addMSecs((quint64) t_msec);
	QString s_date = dt.toString("yyyy-MM-dd hh:mm:ss.zzz");
	ui->absTimeText->setText(s_date);

	if (!m_enableNavTime)
	{
		float posRatio = (float) t_sec / (float) m_recordLength;
		ui->navTimeSlider->setValue((int) (posRatio * 100.0));
	}
}

void FileSourceGui::tick()
{
	if ((++m_tickCount & 0xf) == 0) {
		FileSourceInput::MsgConfigureFileSourceStreamTiming* message = FileSourceInput::MsgConfigureFileSourceStreamTiming::create();
		m_sampleSource->getInputMessageQueue()->push(message);
	}
}
