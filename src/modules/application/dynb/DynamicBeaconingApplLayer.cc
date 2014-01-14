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

#include "DynamicBeaconingApplLayer.h"

Define_Module(DynamicBeaconingApplLayer);

void DynamicBeaconingApplLayer::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);

	std::string myName = getParentModule()->getFullName();

	mac = dynamic_cast<Mac1609_4*>((getParentModule()->getSubmodule("nic")->getSubmodule("mac")));
	if (mac == NULL) {
		error("%s could not find Mac 1609_4 submodule.", myName.c_str());
	}

	radio = dynamic_cast<PhyLayer80211p*>((getParentModule()->getSubmodule("nic")->getSubmodule("phy")));
	if (radio == NULL) {
		error("%s could not find Ieee 802.11p Radio submodule.", myName.c_str());
	}

	if(stage == 1) {

		lastBeaconAt = -1;
		beaconSeqNumber = 0;
		receivedBeacons = 0;
		receivedData = 0;

		neighborExpiry = par("neighborExpiry");
		startBeaconingAt = par("startBeaconingAt");
		neighbors.clear();
		statsNeighbors.clear();
		totalNeighbors.clear();
		lastBusyTime = mac->getBusyTime();

		minBeaconInterval = par("minBeaconInterval");
		maxBeaconInterval = par("maxBeaconInterval");

		neighborsVec.setName("neighbors");
		protoNeighborsVec.setName("protoNeighbors");
		neighborLifetimeVec.setName("neighborLifetime");
		protoNeighborLifetimeVec.setName("protoNeighborLifetime");
		receivedBeaconsSeqNumVec.setName("recvBeaconsSeqNum");
		receivedBeaconsSourceVec.setName("recvBeaconsSource");
		beaconIntervalVec.setName("beaconInterval");
		channelPriorityVec.setName("channelPriority");
		channelBusyRatioVec.setName("channelBusyRatio");
		channelBusyTimeVec.setName("channelBusyTime");
		channelNeighborsVec.setName("channelNeighbors");


		cancelEvent(sendBeaconEvt);
		scheduleAt(simTime() + uniform(0.1, 0.2), sendBeaconEvt);

		DBG << "DynamicBeaconingApplLayer has been initialized!" << std::endl;
	}
}

DynamicBeaconingApplLayer::~DynamicBeaconingApplLayer() {
}

void DynamicBeaconingApplLayer::finish() {
	expireNeighbors(true);
	expireStatsNeighbors(true);
	recordScalar("totalNeighbors", totalNeighbors.size());
}
 
void DynamicBeaconingApplLayer::handleSelfMsg(cMessage* msg) {
	switch (msg->getKind()) {
		case SEND_BEACON_EVT: {
			expireNeighbors();
			expireStatsNeighbors();

			// calculate channel busy ratio (busyTime/totalTime)
			std::pair<simtime_t, simtime_t> busyTime = mac->getBusyTime();
			double busyRatio = (busyTime.first - lastBusyTime.first).dbl()/(busyTime.second - lastBusyTime.second).dbl();
			//std::cerr << "t=" << simTime() << " busyTime: " << busyTime.first << " / " << busyTime.second << std::endl;
			lastBusyTime = busyTime;

			// adapt interval_time
			double b = std::max(0.0, std::min((busyRatio/0.25)-1, 1.0));
			double relInterval = b;
			simtime_t interval_time = minBeaconInterval + relInterval * (minBeaconInterval * neighbors.size());

			// if interval_time still holds...
			if (lastBeaconAt + interval_time <= simTime()) {
				// send beacon
				sendWSM(prepareWSM("beacon", beaconLengthBits, type_CCH, beaconPriority, 0, beaconSeqNumber));
				lastBeaconAt = simTime();
			}

			// schedule next beacon
			scheduleAt(lastBeaconAt + interval_time * uniform(1.00, 1.10), sendBeaconEvt);

			break;
		}
		default: {
			if (msg)
				DBG << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
			break;
		}
	}
}

void DynamicBeaconingApplLayer::sendWSM(WaveShortMessage* wsm) {
	if (simTime() >= startBeaconingAt) {
		BaseWaveApplLayer::sendWSM(wsm);
		beaconSeqNumber++;
	} else {
		delete wsm;
	}
}

void DynamicBeaconingApplLayer::onBeacon(WaveShortMessage* wsm) {
	receivedBeacons++;

	DBG << "Received beacon priority  " << wsm->getPriority() << " at " << simTime() << std::endl;
	int senderId = wsm->getSenderAddress();

	double dist = wsm->getSenderPos().distance(curPosition);

	expireNeighbors();
	expireStatsNeighbors();
	if (neighbors.find(senderId) == neighbors.end()) {
		neighbors[senderId].id = senderId;
		neighbors[senderId].createdAt = simTime();
	}
	neighbors[senderId].dist = dist;
	neighbors[senderId].lastUpdate = simTime();
	if (statsNeighbors.find(senderId) == statsNeighbors.end()) {
		statsNeighbors[senderId].id = senderId;
		statsNeighbors[senderId].createdAt = simTime();
	}
	statsNeighbors[senderId].dist = dist;
	statsNeighbors[senderId].lastUpdate = simTime();
	totalNeighbors.insert(senderId);

}

void DynamicBeaconingApplLayer::onData(WaveShortMessage* wsm) {

	int recipientId = wsm->getRecipientAddress();

	if (recipientId == myId) {
		DBG  << "Received data priority  " << wsm->getPriority() << " at " << simTime() << std::endl;
		receivedData++;
	}
}

void DynamicBeaconingApplLayer::expireNeighbors(bool expireAll) {
	// what would be the maximum interval time I would calculate right now?
	simtime_t interval_time_max = minBeaconInterval + 1.0 * (minBeaconInterval * neighbors.size());

	// (just make sure it's somewhat reasonable)
	interval_time_max = std::min(interval_time_max, simtime_t(60.0));

	// and purge neighbors older than that
	for (Neighbors::iterator i = neighbors.begin(); i != neighbors.end(); ) {
		NeighborEntry& n = i->second;
		if ((simTime() > n.lastUpdate + interval_time_max) || expireAll) {
			neighbors.erase(i++);
		} else {
			i++;
		}
	}

}

void DynamicBeaconingApplLayer::expireStatsNeighbors(bool expireAll) {
	simtime_t interval_time_max = neighborExpiry;

	// and purge neighbors older than that
	for (Neighbors::iterator i = statsNeighbors.begin(); i != statsNeighbors.end(); ) {
		NeighborEntry& n = i->second;
		if ((simTime() > n.lastUpdate + interval_time_max) || expireAll) {
			statsNeighbors.erase(i++);
		} else {
			i++;
		}
	}

}

