#ifndef INCLUDE_LoRaDEMODGUI_H
#define INCLUDE_LoRaDEMODGUI_H

#include <plugin/plugininstanceui.h>
#include "gui/rollupwidget.h"
#include "dsp/channelmarker.h"

#define BANDWIDTHSTRING {7813,15625,20833,31250,62500}

class PluginAPI;
class DeviceSourceAPI;
class ThreadedBasebandSampleSink;
class DownChannelizer;
class LoRaDemod;
class SpectrumVis;

namespace Ui {
	class LoRaDemodGUI;
}

class LoRaDemodGUI : public RollupWidget, public PluginInstanceUI {
	Q_OBJECT

public:
	static LoRaDemodGUI* create(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI);
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
	void viewChanged();
	void on_BW_valueChanged(int value);
	void on_Spread_valueChanged(int value);
	void onWidgetRolled(QWidget* widget, bool rollDown);
	void onMenuDoubleClicked();

private:
	Ui::LoRaDemodGUI* ui;
	PluginAPI* m_pluginAPI;
	DeviceSourceAPI* m_deviceAPI;
	ChannelMarker m_channelMarker;
	bool m_basicSettingsShown;
	bool m_doApplySettings;

	ThreadedBasebandSampleSink* m_threadedChannelizer;
	DownChannelizer* m_channelizer;
	LoRaDemod* m_LoRaDemod;
	SpectrumVis* m_spectrumVis;

	explicit LoRaDemodGUI(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI, QWidget* parent = NULL);
	virtual ~LoRaDemodGUI();

    void blockApplySettings(bool block);
	void applySettings();
};

#endif // INCLUDE_LoRaDEMODGUI_H
