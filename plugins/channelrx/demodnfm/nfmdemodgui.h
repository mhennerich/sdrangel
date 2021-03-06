#ifndef INCLUDE_NFMDEMODGUI_H
#define INCLUDE_NFMDEMODGUI_H

#include <plugin/plugininstanceui.h>
#include "gui/rollupwidget.h"
#include "dsp/dsptypes.h"
#include "dsp/channelmarker.h"
#include "dsp/movingaverage.h"

class PluginAPI;
class DeviceSourceAPI;

class ThreadedBasebandSampleSink;
class DownChannelizer;
class NFMDemod;

namespace Ui {
	class NFMDemodGUI;
}

class NFMDemodGUI : public RollupWidget, public PluginInstanceUI {
	Q_OBJECT

public:
	static NFMDemodGUI* create(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI);
	void destroy();

	void setName(const QString& name);
	QString getName() const;
	virtual qint64 getCenterFrequency() const;
	virtual void setCenterFrequency(qint64 centerFrequency);

	void resetToDefaults();
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);

	virtual bool handleMessage(const Message& message);
	void setCtcssFreq(Real ctcssFreq);

	static const QString m_channelID;

private slots:
	void channelMarkerChanged();
	void on_deltaFrequency_changed(qint64 value);
	void on_rfBW_currentIndexChanged(int index);
	void on_afBW_valueChanged(int value);
	void on_volume_valueChanged(int value);
	void on_squelchGate_valueChanged(int value);
	void on_deltaSquelch_toggled(bool checked);
	void on_squelch_valueChanged(int value);
	void on_ctcss_currentIndexChanged(int index);
	void on_ctcssOn_toggled(bool checked);
	void on_audioMute_toggled(bool checked);
    void on_copyAudioToUDP_toggled(bool checked);
	void onWidgetRolled(QWidget* widget, bool rollDown);
	void onMenuDialogCalled(const QPoint& p);
	void tick();

private:
	Ui::NFMDemodGUI* ui;
	PluginAPI* m_pluginAPI;
	DeviceSourceAPI* m_deviceAPI;
	ChannelMarker m_channelMarker;
	bool m_basicSettingsShown;
	bool m_doApplySettings;

	ThreadedBasebandSampleSink* m_threadedChannelizer;
	DownChannelizer* m_channelizer;
	NFMDemod* m_nfmDemod;
	bool m_ctcssOn;
	bool m_audioMute;
	bool m_squelchOpen;
	uint32_t m_tickCount;

	static const int m_rfBW[];
	static const int m_fmDev[];
	static const int m_nbRfBW;

	explicit NFMDemodGUI(PluginAPI* pluginAPI, DeviceSourceAPI *deviceAPI, QWidget* parent = NULL);
	virtual ~NFMDemodGUI();

	void blockApplySettings(bool block);
	void applySettings(bool force = false);
    void displayUDPAddress();

	void leaveEvent(QEvent*);
	void enterEvent(QEvent*);
};

#endif // INCLUDE_NFMDEMODGUI_H
