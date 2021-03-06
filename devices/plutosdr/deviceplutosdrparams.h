///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 Edouard Griffiths, F4EXB                                   //
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

#ifndef DEVICES_PLUTOSDR_DEVICEPLUTOSDRPARAMS_H_
#define DEVICES_PLUTOSDR_DEVICEPLUTOSDRPARAMS_H_

#include <string>

class DevicePlutoSDRBox;

/**
 * This structure refers to one physical device shared among parties (logical devices represented by
 * the DeviceSinkAPI or DeviceSourceAPI).
 * It allows storing information on the common resources in one place and is shared among participants.
 * There is only one copy that is constructed by the first participant and destroyed by the last.
 * A participant knows it is the first or last by checking the lists of buddies (Rx + Tx).
 */
struct DevicePlutoSDRParams
{
public:
    DevicePlutoSDRParams();
    ~DevicePlutoSDRParams();

    bool open(const std::string& serial);
    void close();

    DevicePlutoSDRBox *getBox() { return m_box; }

private:
    DevicePlutoSDRBox *m_box;
};


#endif /* DEVICES_PLUTOSDR_DEVICEPLUTOSDRPARAMS_H_ */
