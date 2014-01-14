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

#ifndef DYNAMICBEACONINGAPPLLAYER_H_
#define DYNAMICBEACONINGAPPLLAYER_H_

#include "BaseModule.h"
#include <BaseWaveApplLayer.h>
#include <fstream>
#include "mobility/traci/TraCIMobility.h"
#include "mac/ieee80211p/Mac1609_4.h"
#include "phy/PhyLayer80211p.h"

#ifndef DBG
#define DBG EV
#endif

#define DYNAMIC_BEACONING_SNR_THRESHOLD 50.
#define DYNAMIC_BEACONING_NEIGHBOR_MAX_COUNT 50.

class DynamicBeaconingApplLayer : public BaseWaveApplLayer {
	public:
		class NeighborEntry {
			public:
				int id;
				double dist;
				simtime_t createdAt;
				simtime_t lastUpdate;
		};

		virtual ~DynamicBeaconingApplLayer();
		virtual void initialize(int stage);
		virtual void finish();

	protected:
		void handleSelfMsg(cMessage* msg);
		void sendWSM(WaveShortMessage* wsm);
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);
		virtual void expireNeighbors(bool expireAll = false);
		virtual void expireStatsNeighbors(bool expireAll = false);

	protected:
		typedef std::map<int, NeighborEntry> Neighbors;
		typedef std::set<int> TotalNeighbors;

		simtime_t lastBeaconAt;
		uint32_t beaconSeqNumber;

		uint32_t receivedBeacons;
		uint32_t receivedData;

		Neighbors neighbors;
		Neighbors statsNeighbors;
		TotalNeighbors totalNeighbors;
		simtime_t neighborExpiry;
		simtime_t startBeaconingAt;
		std::pair<simtime_t, simtime_t> lastBusyTime;

		Mac1609_4* mac;
		PhyLayer80211p* radio;
	        double minBeaconInterval;  // minimum time in seconds to wait before sending a beacon  
        	double maxBeaconInterval;  // maximum time in seconds to wait before sending a beacon

		mutable cOutVector neighborsVec;
		mutable cOutVector protoNeighborsVec;
		mutable cOutVector neighborLifetimeVec;
		mutable cOutVector protoNeighborLifetimeVec;
		mutable cOutVector receivedBeaconsSeqNumVec;
		mutable cOutVector receivedBeaconsSourceVec;
		mutable cOutVector beaconIntervalVec;
		mutable cOutVector channelPriorityVec;
		mutable cOutVector channelBusyTimeVec;
		mutable cOutVector channelBusyRatioVec;
		mutable cOutVector channelNeighborsVec;
};

#endif /* DYNAMICBEACONINGAPPLLAYER_H_ */
