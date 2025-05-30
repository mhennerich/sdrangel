///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 maintech GmbH, Otto-Hahn-Str. 15, 97204 Hoechberg, Germany //
// written by Christian Daniel                                                   //
// Copyright (C) 2015-2022 Edouard Griffiths, F4EXB <f4exb06@gmail.com>          //
// Copyright (C) 2019 Davide Gerhard <rainbow@irh.it>                            //
// Copyright (C) 2020 Kacper Michajłow <kasper93@gmail.com>                      //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include "plugin/pluginapi.h"

#ifndef SERVER_MODE
#include "filesourcegui.h"
#endif
#include "filesource.h"
#include "filesourcewebapiadapter.h"
#include "filesourceplugin.h"

const PluginDescriptor FileSourcePlugin::m_pluginDescriptor = {
    FileSource::m_channelId,
    QStringLiteral("File channel source"),
    QStringLiteral("7.22.5"),
    QStringLiteral("(c) Edouard Griffiths, F4EXB"),
    QStringLiteral("https://github.com/f4exb/sdrangel"),
    true,
    QStringLiteral("https://github.com/f4exb/sdrangel")
};

FileSourcePlugin::FileSourcePlugin(QObject* parent) :
    QObject(parent),
    m_pluginAPI(0)
{
}

const PluginDescriptor& FileSourcePlugin::getPluginDescriptor() const
{
    return m_pluginDescriptor;
}

void FileSourcePlugin::initPlugin(PluginAPI* pluginAPI)
{
    m_pluginAPI = pluginAPI;

    // register source
    m_pluginAPI->registerTxChannel(FileSource::m_channelIdURI, FileSource::m_channelId, this);
}

void FileSourcePlugin::createTxChannel(DeviceAPI *deviceAPI, BasebandSampleSource **bs, ChannelAPI **cs) const
{
	if (bs || cs)
	{
		FileSource *instance = new FileSource(deviceAPI);

		if (bs) {
			*bs = instance;
		}

		if (cs) {
			*cs = instance;
		}
	}
}

#ifdef SERVER_MODE
ChannelGUI* FileSourcePlugin::createTxChannelGUI(
        DeviceUISet *deviceUISet,
        BasebandSampleSource *txChannel) const
{
	(void) deviceUISet;
	(void) txChannel;
    return nullptr;
}
#else
ChannelGUI* FileSourcePlugin::createTxChannelGUI(DeviceUISet *deviceUISet, BasebandSampleSource *txChannel) const
{
    return FileSourceGUI::create(m_pluginAPI, deviceUISet, txChannel);
}
#endif

ChannelWebAPIAdapter* FileSourcePlugin::createChannelWebAPIAdapter() const
{
	return new FileSourceWebAPIAdapter();
}
