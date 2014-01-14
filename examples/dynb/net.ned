//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package org.mixim.examples.dynb;

import org.mixim.base.connectionManager.ConnectionManager;
import org.mixim.base.modules.BaseWorldUtility;
import org.mixim.modules.world.annotations.AnnotationManager;
import org.mixim.modules.obstacle.ObstacleControl;
import org.mixim.modules.mobility.traci.TraCIScenarioManagerLaunchd;

network net
{
    parameters:
	int numHosts;

    submodules:
        baseWorldUtility: BaseWorldUtility {
            @display("p=70,20");
        }
        connectionManager: ConnectionManager {
            @display("p=70,91");
        }
        obstacles: ObstacleControl {
            @display("p=240,50");
        }
        annotations: AnnotationManager {
            @display("p=260,50");
        }
        host[numHosts]: Node;

    connections allowunconnected:
}
