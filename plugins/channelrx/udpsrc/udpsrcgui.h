///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 F4EXB                                                      //
// written by Edouard Griffiths                                                  //
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

#ifndef INCLUDE_UDPSRCGUI_H
#define INCLUDE_UDPSRCGUI_H

#include <plugin/plugininstanceui.h>
#include <QHostAddress>
#include "gui/rollupwidget.h"
#include "dsp/channelmarker.h"
#include "dsp/movingaverage.h"

#include "udpsrc.h"

class PluginAPI;
class DeviceSourceAPI;
class ThreadedBasebandSampleSink;
class DownChannelizer;
class UDPSrc;
class SpectrumVis;

namespace Ui {
	class UDPSrcGUI;
}

class UDPSrcGUI : public RollupWidget, public PluginInstanceUI {
	Q_OBJECT

public:
	static UDPSrcGUI* create(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI);
	void destroy();

	void setName(const QString& name);
	QString getName() const;
	virtual qint64 getCenterFrequency() const;
	virtual void setCenterFrequency(qint64 centerFrequency);

	void resetToDefaults();
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);

	virtual bool handleMessage(const Message& message);

	static const QString m_channelID;

private slots:
	void channelMarkerChanged();
	void on_deltaFrequency_changed(qint64 value);
	void on_sampleFormat_currentIndexChanged(int index);
	void on_sampleRate_textEdited(const QString& arg1);
	void on_rfBandwidth_textEdited(const QString& arg1);
	void on_udpAddress_textEdited(const QString& arg1);
	void on_udpPort_textEdited(const QString& arg1);
	void on_audioPort_textEdited(const QString& arg1);
	void on_fmDeviation_textEdited(const QString& arg1);
	void on_audioActive_toggled(bool active);
	void on_audioStereo_toggled(bool stereo);
	void on_applyBtn_clicked();
	void onWidgetRolled(QWidget* widget, bool rollDown);
	void onMenuDialogCalled(const QPoint& p);
	void on_gain_valueChanged(int value);
	void on_volume_valueChanged(int value);
	void on_squelch_valueChanged(int value);
	void on_squelchGate_valueChanged(int value);
	void on_agc_toggled(bool agc);
	void tick();

private:
	Ui::UDPSrcGUI* ui;
	PluginAPI* m_pluginAPI;
	DeviceSourceAPI* m_deviceAPI;
	UDPSrc* m_udpSrc;
	ChannelMarker m_channelMarker;
	MovingAverage<double> m_channelPowerAvg;
    MovingAverage<double> m_inPowerAvg;
	uint32_t m_tickCount;

	// settings
	UDPSrc::SampleFormat m_sampleFormat;
	Real m_outputSampleRate;
	Real m_rfBandwidth;
	int m_fmDeviation;
	Real m_gain;
	bool m_audioActive;
	bool m_audioStereo;
	int m_volume;
	bool m_doApplySettings;

	// RF path
	ThreadedBasebandSampleSink* m_threadedChannelizer;
	DownChannelizer* m_channelizer;
	SpectrumVis* m_spectrumVis;

	explicit UDPSrcGUI(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI, QWidget* parent = 0);
	virtual ~UDPSrcGUI();

    void blockApplySettings(bool block);
	void applySettings(bool force = false);
	void applySettingsImmediate(bool force = false);
	void displaySettings();

	void leaveEvent(QEvent*);
	void enterEvent(QEvent*);
};

#endif // INCLUDE_UDPSRCGUI_H
