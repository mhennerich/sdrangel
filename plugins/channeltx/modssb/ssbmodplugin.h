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

#ifndef INCLUDE_SSBMODPLUGIN_H
#define INCLUDE_SSBMODPLUGIN_H

#include <QObject>
#include "plugin/plugininterface.h"

class DeviceSinkAPI;

class SSBModPlugin : public QObject, PluginInterface {
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "sdrangel.channeltx.ssbmod")

public:
	explicit SSBModPlugin(QObject* parent = NULL);

	const PluginDescriptor& getPluginDescriptor() const;
	void initPlugin(PluginAPI* pluginAPI);

	PluginInstanceUI* createTxChannel(const QString& channelName, DeviceSinkAPI *deviceAPI);

private:
	static const PluginDescriptor m_pluginDescriptor;

	PluginAPI* m_pluginAPI;

private slots:
	void createInstanceModSSB(DeviceSinkAPI *deviceAPI);
};

#endif // INCLUDE_SSBMODPLUGIN_H
