///////////////////////////////////////////////////////////////////////////////////
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
#include "ambegui.h"
#endif
#include "ambe.h"
#include "ambeplugin.h"
#include "ambewebapiadapter.h"

const PluginDescriptor AMBEPlugin::m_pluginDescriptor = {
    AMBE::m_featureId,
	QStringLiteral("AMBE Controller"),
    QStringLiteral("7.22.7"),
	QStringLiteral("(c) Edouard Griffiths, F4EXB"),
	QStringLiteral("https://github.com/f4exb/sdrangel"),
	true,
	QStringLiteral("https://github.com/f4exb/sdrangel")
};

AMBEPlugin::AMBEPlugin(QObject* parent) :
	QObject(parent),
	m_pluginAPI(nullptr)
{
}

const PluginDescriptor& AMBEPlugin::getPluginDescriptor() const
{
	return m_pluginDescriptor;
}

void AMBEPlugin::initPlugin(PluginAPI* pluginAPI)
{
	m_pluginAPI = pluginAPI;

	// register Simple PTT feature
	m_pluginAPI->registerFeature(AMBE::m_featureIdURI, AMBE::m_featureId, this);
}

#ifdef SERVER_MODE
FeatureGUI* AMBEPlugin::createFeatureGUI(FeatureUISet *featureUISet, Feature *feature) const
{
	(void) featureUISet;
	(void) feature;
    return nullptr;
}
#else
FeatureGUI* AMBEPlugin::createFeatureGUI(FeatureUISet *featureUISet, Feature *feature) const
{
	return AMBEGUI::create(m_pluginAPI, featureUISet, feature);
}
#endif

Feature* AMBEPlugin::createFeature(WebAPIAdapterInterface* webAPIAdapterInterface) const
{
    return new AMBE(webAPIAdapterInterface);
}

FeatureWebAPIAdapter* AMBEPlugin::createFeatureWebAPIAdapter() const
{
	return new AMBEWebAPIAdapter();
}
