//
//
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@informatik.uni-erlangen.de>
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

package org.mixim.examples.dynb;
import org.mixim.base.modules.BaseArp;
import org.mixim.base.modules.IBaseApplLayer;
import org.mixim.modules.mobility.CircleMobility;
import org.mixim.modules.nic.WirelessNic;
import org.mixim.base.modules.IMobility;

module Node
{
    parameters:
        string applicationType = default("TestWaveApplLayer");
        string mobilityType = default("CircleMobility");
        @display("i=status/busy");
    gates:
        input radioIn; // gate for sendDirect
    submodules:
        appl: <applicationType> like IBaseApplLayer {
            parameters:
                @display("p=70,30;i=app");
        }

        arp: BaseArp {
            parameters:
                @display("p=130,108");
        }

        nic: WirelessNic {
            parameters:
                @display("p=60,166");
        }

        mobility: <mobilityType> like IMobility {
            parameters:
                @display("p=130,172;i=block/cogwheel");
        }
    connections:
        nic.upperLayerOut --> appl.lowerLayerIn;
        nic.upperLayerIn <-- appl.lowerLayerOut;
        nic.upperControlOut --> appl.lowerControlIn;
        nic.upperControlIn <-- appl.lowerControlOut;

        radioIn --> nic.radioIn;

}
