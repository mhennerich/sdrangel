#include "../../channelrx/demodssb/ssbplugin.h"

#include <device/devicesourceapi.h>
#include <QtPlugin>
#include "plugin/pluginapi.h"
#include "../../channelrx/demodssb/ssbdemodgui.h"

const PluginDescriptor SSBPlugin::m_pluginDescriptor = {
	QString("SSB Demodulator"),
	QString("3.5.4"),
	QString("(c) Edouard Griffiths, F4EXB"),
	QString("https://github.com/f4exb/sdrangel"),
	true,
	QString("https://github.com/f4exb/sdrangel")
};

SSBPlugin::SSBPlugin(QObject* parent) :
	QObject(parent),
	m_pluginAPI(0)
{
}

const PluginDescriptor& SSBPlugin::getPluginDescriptor() const
{
	return m_pluginDescriptor;
}

void SSBPlugin::initPlugin(PluginAPI* pluginAPI)
{
	m_pluginAPI = pluginAPI;

	// register demodulator
	m_pluginAPI->registerRxChannel(SSBDemodGUI::m_channelID, this);
}

PluginInstanceUI* SSBPlugin::createRxChannel(const QString& channelName, DeviceSourceAPI *deviceAPI)
{
	if(channelName == SSBDemodGUI::m_channelID)
	{
		SSBDemodGUI* gui = SSBDemodGUI::create(m_pluginAPI, deviceAPI);
		return gui;
	} else {
		return NULL;
	}
}

void SSBPlugin::createInstanceSSB(DeviceSourceAPI *deviceAPI)
{
	SSBDemodGUI::create(m_pluginAPI, deviceAPI);
}
