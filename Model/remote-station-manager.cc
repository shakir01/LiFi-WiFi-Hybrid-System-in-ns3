#include "remote-station-manager.h"

 namespace  ns3
{
    TypeId RemoteStationManager::GetTypeId (void)
    {
        static TypeId tid = TypeId ("ns3:RemoteStationManager").SetParent<Object> ().AddConstructor<RemoteStationManager> ();
        return tid;
    }

    void RemoteStationManager::AddMac(Ptr<LiFiRegularMac> mac, Mac48Address macAddress)
    {
        regularMacMap.insert(std::make_pair(macAddress,mac));
    }

    ns3::Ptr<LiFiRegularMac> RemoteStationManager::GetMac(Mac48Address macAddress)
    {
        std::map<Mac48Address, Ptr<LiFiRegularMac> >::iterator it1;
        it1 = regularMacMap.find(macAddress);
        if (it1 != regularMacMap.end())
        {
            Ptr<LiFiRegularMac> mac = it1->second;
            return mac;
        }

        return 0;
    }
} 

