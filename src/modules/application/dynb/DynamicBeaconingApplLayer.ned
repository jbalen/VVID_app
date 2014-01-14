//
// Copyright (C) 2012 Stefan Joerer <stefan.joerer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package org.mixim.modules.application.dynb;
import org.mixim.modules.application.ieee80211p.BaseWaveApplLayer;


simple DynamicBeaconingApplLayer extends BaseWaveApplLayer
{
    parameters:
        @class(DynamicBeaconingApplLayer);
        double neighborExpiry @unit("s") = default(1.0s);  // time after which a neighbor entry expires
        double startBeaconingAt @unit("s") = default(1.0s);
        double minBeaconInterval @unit("s");  // minimum time in seconds to wait before sending a beacon
        double maxBeaconInterval @unit("s");  // maximum time in seconds to wait before sending a beacon
}
