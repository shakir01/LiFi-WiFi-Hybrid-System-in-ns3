#include "PerformanceAnalysis.h"
namespace ns3 {
PerformanceAnalysis::PerformanceAnalysis ()
{
}


void  PerformanceAnalysis::SetLiFiKeys (std::string key)
{
  lifiAPKeys.push_back(key);
}

void PerformanceAnalysis::SwitchUserFromLiFiToWifi (std::string wifi_key ,Ptr<PacketSink> sink)
{
    /*//Clear Keys from LiFi Keys and add it to WiFi Keys
    size_t itemIndex = -1;
    for (size_t i = 0; i < lifiAPKeys.size(); i++)
    {
      if (lifiAPKeys[i] == lifi_key)
      {
        itemIndex = i;
            std::cout<<"void PerformanceAnalysis::SwitchUserFromLiFiToWifi (std::string key, Ptr<PacketSink> sink) "<<itemIndex<<" lifiAPKeys "<<lifiAPKeys.size()<<" lifiAPKeys[i]  "<<lifiAPKeys[i] <<std::endl;

      // break;
      }
    }
    if (itemIndex >= 0)
    {
      //std::cout<<"void PerformanceAnalysis::SwitchUserFromLiFiToWifi (std::string key, Ptr<PacketSink> sink) "<<itemIndex<<" lifiAPKeys "<<lifiAPKeys.size()<<std::endl;
        lifiAPKeys.erase(lifiAPKeys.begin() + itemIndex);
    }
    */

    bool keyFound = false;
    for (size_t i = 0; i < wifiAPKeys.size(); i++)
    {
        if (wifiAPKeys[i] == wifi_key)
        {
          keyFound = true;
          break;
        }    
    }

   if (!keyFound)
   {
      SetWiFiKeys(wifi_key);
   }
 
  //Update Sink Applications
  ApplicationContainer cont;
  cont = lifiStaSinkApps;

  ApplicationContainer c;
  lifiStaSinkApps = c;
  for (size_t i = 0; i < cont.GetN(); i++)
  {
    if (cont.Get(i)->GetNode()->GetId() != sink->GetNode()->GetId())
    {
      lifiStaSinkApps.Add(cont.Get(i));
    }
    else 
    {
      wifiStaSinkApps.Add(cont.Get(i));
    }  
  }

  if (lifiStaSinkApps.GetN() == 0)
  {
    std::cout<<"if (lifiStaSinkApps.GetN() == 0) "<<std::endl;
      lifiAPKeys.clear();
  }
  
  SetLiFiSinkApplicationContainer(lifiStaSinkApps);
}

void  PerformanceAnalysis::SetWiFiKeys (std::string key)
{
  wifiAPKeys.push_back (key);
}

std::vector<std::string>  PerformanceAnalysis::GetLiFiKeys ()
{
  return lifiAPKeys;
}

std::vector<std::string> PerformanceAnalysis::GetWiFiKeys ()
{
  return wifiAPKeys;
}

void PerformanceAnalysis::SetLiFiSinkApplicationContainer (ApplicationContainer appContainer)
{
  lifiStaSinkApps.Add(appContainer);
}
void PerformanceAnalysis::SetLiFiCsmaClientApplicationContainer (ApplicationContainer appContainer)
{
  lifiCsmaClientApps.Add(appContainer);
}

void PerformanceAnalysis::SetWiFiStaSinkApplicationContainer (ApplicationContainer appContainer)
{
  wifiStaSinkApps.Add(appContainer);
}

void PerformanceAnalysis::SetWiFiCsmaClientsApplicationContainer (ApplicationContainer appContainer)
{
  wifiCsmaClientApps.Add(appContainer);
}

ApplicationContainer PerformanceAnalysis::CetLiFiSinkApplicationContainer ()
{
  return lifiStaSinkApps;
}

ApplicationContainer PerformanceAnalysis::GetLiFiCsmaClientApplicationContainer ()
{
  return lifiCsmaClientApps;
}

ApplicationContainer PerformanceAnalysis::GetWiFiStaSinkApplicationContainer ()
{
  return wifiStaSinkApps;

}
ApplicationContainer PerformanceAnalysis::GetWiFiCsmaClientsApplicationContainer ()
{
  return wifiCsmaClientApps;
}

Ptr<PacketSink> PerformanceAnalysis::GetDestinationApplicationForIpAddress (Ipv4Address destAddress)
{
   ApplicationContainer appContainer;
   appContainer.Add(wifiStaSinkApps);
   appContainer.Add(lifiStaSinkApps);

   for (uint32_t i = 0; i < appContainer.GetN(); i++)
   {
     Ptr<PacketSink> packetSink = ns3::DynamicCast<ns3::PacketSink> (appContainer.Get(i));
     std::vector<Connection> connectVector = packetSink->GetConnections();

     for (size_t jj = 0; jj < connectVector.size(); jj++)
     {
       Connection conn = connectVector [jj];
      // std::cout<<"conn.destinationIpAddress "<<conn.destinationIpAddress<<std::endl;
       if (conn.destinationIpAddress == destAddress)
       {
         return packetSink;
       } 
     }
   }
  
  return 0;
}

bool PerformanceAnalysis::ShouldCaptureTrafficforPort (std::string keys)
{
bool found = false;
for (size_t i = 0; i < lifiAPKeys.size(); i++)
{
  if (lifiAPKeys[i] == keys)
  {
    found = true;
  }
}

for (size_t i = 0; i < wifiAPKeys.size(); i++)
{
  if (wifiAPKeys[i] == keys)
  {
    found = true;
  }
}

return found;
}

//std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > >  PerformanceAnalysis::CheckThroughput (Ptr<FlowMonitor>  m_monitor, FlowMonitorHelper&  m_flowmon)
std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > >  PerformanceAnalysis::CheckThroughput (Ptr<Ipv4FlowClassifier> classifier, FlowMonitor::FlowStatsContainer stats)
{
   std::map<std::string, std::vector<LiFiWiFiFlowStats> > lifiApStatsVectorsMap;//contains all statistic for all lifi APs
   std::map<std::string, std::vector<LiFiWiFiFlowStats> > wifiApStatsVectorsMap;
   std::map<std::string, std::vector<LiFiWiFiFlowStats> > handoverDevicesVectorMap;


   std::vector<LiFiWiFiFlowStats> handoverFlowsStatsVector;
   std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > > lifiWiFiAPsSinksVector;
   double sum_delay_wifi = 0;
   for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
   {
     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
     std::ostringstream stream;
     stream<<t.destinationPort;
     std::string keyStr =  stream.str();

     if (ShouldCaptureTrafficforPort(keyStr) == false) continue;

     std::cout<<"destinationPort port "<<keyStr<<std::endl;

     bool wifiSinkAddressFound = false;
     bool lifiSinkAddressFound = false;

     for (uint32_t j=0; j<lifiAPKeys.size(); j++)
     {
       if (keyStr == lifiAPKeys[j])
       {
          lifiSinkAddressFound = true;
          break;
       }
     }

     for (uint32_t j=0; j<wifiAPKeys.size(); j++)
     {
       if (keyStr == wifiAPKeys[j])
       {
          wifiSinkAddressFound = true;
          break;
       }
     }

    if (lifiSinkAddressFound == false && wifiSinkAddressFound == false) continue;
    
    if (lifiSinkAddressFound == true)
     {
       Ptr<PacketSink> lifiSinkApp = 0;
       LiFiWiFiFlowStats lifiStats;

      for (size_t j = 0; j < lifiStaSinkApps.GetN(); j++)
      {
          Ptr<PacketSink> sink = DynamicCast<PacketSink> (lifiStaSinkApps.Get(j));
          Ptr <Node> node      = sink->GetNode();
          Ptr<Ipv4> ipv4A      = node->GetObject<Ipv4> ();
          Ipv4Address ipv4     = ipv4A->GetAddress(2,0).GetLocal();

          if (t.destinationAddress == ipv4)
          {
            lifiSinkApp  = DynamicCast<PacketSink> (lifiStaSinkApps.Get(j));
          }
      }
    
      lifiStats.sinkApp = lifiSinkApp;
      lifiStats.sourceAddress = t.sourceAddress;
      lifiStats.destinationAddress = t.destinationAddress;

      lifiStats.sunDelay = i->second.delaySum.GetSeconds();
      lifiStats.sumJitters = i->second.jitterSum.GetSeconds();
      lifiStats.totalRxBytes = i->second.rxBytes;
      if (i->second.rxPackets !=0)
      {
          lifiStats.perPacketDelay = lifiStats.sunDelay/ i->second.rxPackets;
          lifiStats.perPacketJitter =   lifiStats.sumJitters/ i->second.rxPackets;
      }

      lifiStats.totalDroppedPackets = i->second.lostPackets;
      lifiStats.totalLostBytes = (i->second.txBytes - i->second.rxBytes);
      lifiStats.totalRetransmission = i->second.timesForwarded;

      lifiStats.rxPackets = i->second.rxPackets;
      lifiStats.txPackets = i->second.txPackets;

      lifiStats.duration  = (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ());
      lifiStats.throughput = (i->second.rxBytes * 8)/((double)lifiStats.duration*1e6);

      if (lifiSinkApp != 0)
      {
          std::map<std::string, std::vector<LiFiWiFiFlowStats> >:: iterator it1;
          it1 = lifiApStatsVectorsMap.find(keyStr);
          std::vector<LiFiWiFiFlowStats> lifiFlowStatsVect;
          if (it1 != lifiApStatsVectorsMap.end())
          {
              lifiFlowStatsVect = it1->second;
              lifiFlowStatsVect.push_back(lifiStats);
              lifiApStatsVectorsMap[keyStr] = lifiFlowStatsVect;
          }
          else
          {
              lifiFlowStatsVect.push_back (lifiStats);
              lifiApStatsVectorsMap.insert(make_pair(keyStr, lifiFlowStatsVect));
          }   
      }
      else 
      {
          handoverFlowsStatsVector.push_back(lifiStats);
      }   
   }
  else if (wifiSinkAddressFound == true)
    {
        ns3::Ptr<PacketSink> wifiSinkApp;
        LiFiWiFiFlowStats wifiStats;

        for (size_t j = 0; j < wifiStaSinkApps.GetN(); j++)
        {
            Ptr<PacketSink> sink = DynamicCast<PacketSink> (wifiStaSinkApps.Get(j));
            Ptr <Node> node      = sink->GetNode();
            Ptr<Ipv4> ipv4A      = node->GetObject<Ipv4> ();
            Ipv4Address ipv4     = ipv4A->GetAddress(1,0).GetLocal();
            if (t.destinationAddress == ipv4)
            {
              wifiSinkApp  = ns3::DynamicCast<PacketSink> (wifiStaSinkApps.Get(j));
            }
        }

        wifiStats.sinkApp            = wifiSinkApp;
        wifiStats.sourceAddress      = t.sourceAddress;
        wifiStats.destinationAddress = t.destinationAddress;
        wifiStats.sunDelay           = i->second.delaySum.GetSeconds();
        wifiStats.sumJitters         = i->second.jitterSum.GetSeconds();
        wifiStats.rxPackets          = i->second.rxPackets;
        wifiStats.txPackets          = i->second.txPackets;
        wifiStats.totalRxBytes       = i->second.rxBytes;
        if ( i->second.rxPackets !=0)
        {
            wifiStats.perPacketDelay = wifiStats.sunDelay/(i->second.rxPackets-1);
            sum_delay_wifi = sum_delay_wifi + wifiStats.perPacketDelay;
            wifiStats.perPacketJitter =   wifiStats.sumJitters/ (i->second.rxPackets-1);
        }

        wifiStats.totalDroppedPackets = i->second.lostPackets;
        wifiStats.totalLostBytes = (i->second.txBytes - i->second.rxBytes);
        wifiStats.totalRetransmission = i->second.timesForwarded;
        wifiStats.duration  = (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ());
        wifiStats.throughput = (i->second.rxBytes * 8)/((double)wifiStats.duration*1e6);

        std::map<std::string, std::vector<LiFiWiFiFlowStats> >:: iterator it1;
        it1 = wifiApStatsVectorsMap.find(keyStr);
        std::vector<LiFiWiFiFlowStats> wifiFlowStatsVect;
        if (it1 != wifiApStatsVectorsMap.end())
        {
            wifiFlowStatsVect = it1->second;
            wifiFlowStatsVect.push_back(wifiStats);
            wifiApStatsVectorsMap[keyStr] = wifiFlowStatsVect;
        }
        else
        {
          wifiFlowStatsVect.push_back (wifiStats);
          wifiApStatsVectorsMap.insert(make_pair(keyStr, wifiFlowStatsVect));
        }
     }
  }

  handoverDevicesVectorMap.insert(make_pair("hadover_apps",handoverFlowsStatsVector));
  std::cout<<"PerformanceAnalysis::CheckThroughput WiF Mean Delay  "<<(sum_delay_wifi/33)<<std::endl;
  lifiWiFiAPsSinksVector.push_back (handoverDevicesVectorMap);
  lifiWiFiAPsSinksVector.push_back (lifiApStatsVectorsMap);
  lifiWiFiAPsSinksVector.push_back (wifiApStatsVectorsMap);
  return lifiWiFiAPsSinksVector;
}
}
