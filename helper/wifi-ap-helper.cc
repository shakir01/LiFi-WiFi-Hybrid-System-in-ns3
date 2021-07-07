/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Mirko Banchi <mk.banchi@gmail.com>
 */

#include "wifi-ap-helper.h"
#include "ns3/wifi-ap-net-device.h"
#include "ns3/wifi-mac.h"
#include "ns3/regular-wifi-mac.h"
//#include "ns3/dca-txop.h"
//#include "ns3/edca-txop-n.h"
#include "ns3/qos-utils.h"
#include "ns3/minstrel-wifi-manager.h"
#include "ns3/ap-wifi-mac.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-remote-station-manager.h"
#include "ns3/channel.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/names.h"
#include "ns3/wifi-ap-net-device.h"

#include "ns3/wifi-net-device.h"
#include "ns3/minstrel-wifi-manager.h"
#include "ns3/minstrel-ht-wifi-manager.h"
#include "ns3/ap-wifi-mac.h"
#include "ns3/ampdu-subframe-header.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/radiotap-header.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include "ns3/net-device-queue-interface.h"
#include "ns3/wifi-mac-queue.h"
#include "ns3/qos-utils.h"
#include "ns3/ht-configuration.h"
#include "ns3/vht-configuration.h"
#include "ns3/he-configuration.h"
#include "ns3/obss-pd-algorithm.h"
#include "ns3/wifi-ack-policy-selector.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("WifiApHelper");

WifiApHelper::  WifiApHelper ()
{
WifiHelper ();
}

/*NetDeviceContainer
WifiApHelper::Install (const WifiPhyHelper &phyHelper,
                     const WifiMacHelper &macHelper, NodeContainer c) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<WifiApNetDevice> device = CreateObject<WifiApNetDevice> ();
      Ptr<WifiRemoteStationManager> manager = m_stationManager.Create<WifiRemoteStationManager> ();
      Ptr<WifiMac> mac = macHelper.Create ();
      Ptr<WifiPhy> phy = phyHelper.Create (node, device);
      mac->SetAddress (Mac48Address::Allocate ());
      mac->ConfigureStandard (m_standard);
      phy->ConfigureStandard (m_standard);
      device->SetMac (mac);
      device->SetPhy (phy);
      device->SetRemoteStationManager (manager);
      node->AddDevice (device);
      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ());
    }
  return devices;
}*/
NetDeviceContainer
WifiApHelper::Install (const WifiPhyHelper &phyHelper,
                     const WifiMacHelper &macHelper,
                     NodeContainer::Iterator first,
                     NodeContainer::Iterator last) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = first; i != last; ++i)
    {
      Ptr<Node> node = *i;
      Ptr<WifiApNetDevice> device = CreateObject<WifiApNetDevice> ();
      if (m_standard >= WIFI_PHY_STANDARD_80211n_2_4GHZ)
        {
          Ptr<HtConfiguration> htConfiguration = CreateObject<HtConfiguration> ();
          device->SetHtConfiguration (htConfiguration);
        }
      if ((m_standard == WIFI_PHY_STANDARD_80211ac) || (m_standard == WIFI_PHY_STANDARD_80211ax_5GHZ))
        {
          Ptr<VhtConfiguration> vhtConfiguration = CreateObject<VhtConfiguration> ();
          device->SetVhtConfiguration (vhtConfiguration);
        }
      if (m_standard >= WIFI_PHY_STANDARD_80211ax_2_4GHZ)
        {
          Ptr<HeConfiguration> heConfiguration = CreateObject<HeConfiguration> ();
          device->SetHeConfiguration (heConfiguration);
        }
      Ptr<WifiRemoteStationManager> manager = m_stationManager.Create<WifiRemoteStationManager> ();
      Ptr<WifiMac> mac = macHelper.Create (device);
      Ptr<WifiPhy> phy = phyHelper.Create (node, device);
      mac->SetAddress (Mac48Address::Allocate ());
      mac->ConfigureStandard (m_standard);
      phy->ConfigureStandard (m_standard);
      device->SetMac (mac);
      device->SetPhy (phy);
      device->SetRemoteStationManager (manager);
      node->AddDevice (device);
      if ((m_standard >= WIFI_PHY_STANDARD_80211ax_2_4GHZ) && (m_obssPdAlgorithm.IsTypeIdSet ()))
        {
          Ptr<ObssPdAlgorithm> obssPdAlgorithm = m_obssPdAlgorithm.Create<ObssPdAlgorithm> ();
          device->AggregateObject (obssPdAlgorithm);
          obssPdAlgorithm->ConnectWifiNetDevice (device);
        }
      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ());
      // Aggregate a NetDeviceQueueInterface object if a RegularWifiMac is installed
      Ptr<RegularWifiMac> rmac = DynamicCast<RegularWifiMac> (mac);
      if (rmac)
        {
          Ptr<NetDeviceQueueInterface> ndqi;
          BooleanValue qosSupported;
          PointerValue ptr;
          Ptr<WifiMacQueue> wmq;
          Ptr<WifiAckPolicySelector> ackSelector;

          rmac->GetAttributeFailSafe ("QosSupported", qosSupported);
          if (qosSupported.Get ())
            {
              ndqi = CreateObjectWithAttributes<NetDeviceQueueInterface> ("NTxQueues",
                                                                          UintegerValue (4));

              rmac->GetAttributeFailSafe ("BE_Txop", ptr);
              ackSelector = m_ackPolicySelector[AC_BE].Create<WifiAckPolicySelector> ();
              ackSelector->SetQosTxop (ptr.Get<QosTxop> ());
              ptr.Get<QosTxop> ()->SetAckPolicySelector (ackSelector);
              wmq = ptr.Get<QosTxop> ()->GetWifiMacQueue ();
              ndqi->GetTxQueue (0)->ConnectQueueTraces (wmq);

              rmac->GetAttributeFailSafe ("BK_Txop", ptr);
              ackSelector = m_ackPolicySelector[AC_BK].Create<WifiAckPolicySelector> ();
              ackSelector->SetQosTxop (ptr.Get<QosTxop> ());
              ptr.Get<QosTxop> ()->SetAckPolicySelector (ackSelector);
              wmq = ptr.Get<QosTxop> ()->GetWifiMacQueue ();
              ndqi->GetTxQueue (1)->ConnectQueueTraces (wmq);

              rmac->GetAttributeFailSafe ("VI_Txop", ptr);
              ackSelector = m_ackPolicySelector[AC_VI].Create<WifiAckPolicySelector> ();
              ackSelector->SetQosTxop (ptr.Get<QosTxop> ());
              ptr.Get<QosTxop> ()->SetAckPolicySelector (ackSelector);
              wmq = ptr.Get<QosTxop> ()->GetWifiMacQueue ();
              ndqi->GetTxQueue (2)->ConnectQueueTraces (wmq);

              rmac->GetAttributeFailSafe ("VO_Txop", ptr);
              ackSelector = m_ackPolicySelector[AC_VO].Create<WifiAckPolicySelector> ();
              ackSelector->SetQosTxop (ptr.Get<QosTxop> ());
              ptr.Get<QosTxop> ()->SetAckPolicySelector (ackSelector);
              wmq = ptr.Get<QosTxop> ()->GetWifiMacQueue ();
              ndqi->GetTxQueue (3)->ConnectQueueTraces (wmq);
              ndqi->SetSelectQueueCallback (m_selectQueueCallback);
            }
          else
            {
              ndqi = CreateObject<NetDeviceQueueInterface> ();

              rmac->GetAttributeFailSafe ("Txop", ptr);
              wmq = ptr.Get<Txop> ()->GetWifiMacQueue ();
              ndqi->GetTxQueue (0)->ConnectQueueTraces (wmq);
            }
          device->AggregateObject (ndqi);
        }
    }
  return devices;
}

NetDeviceContainer
WifiApHelper::Install (const WifiPhyHelper &phy,
                     const WifiMacHelper &mac, Ptr<Node> node) const
{
  return Install (phy, mac, NodeContainer (node));
}

NetDeviceContainer
WifiApHelper::Install (const WifiPhyHelper &phyHelper,
                     const WifiMacHelper &macHelper, NodeContainer c) const
{
  return Install (phyHelper, macHelper, c.Begin (), c.End ());
}

NetDeviceContainer
WifiApHelper::Install (const WifiPhyHelper &phy,
                     const WifiMacHelper &mac, std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (phy, mac, NodeContainer (node));
}

/*int64_t
WifiApHelper::AssignStreams (NetDeviceContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  Ptr<NetDevice> netDevice;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      netDevice = (*i);
      Ptr<WifiApNetDevice> wifi = DynamicCast<WifiApNetDevice> (netDevice);
      if (wifi)
        {
          //Handle any random numbers in the PHY objects.
          currentStream += wifi->GetPhy ()->AssignStreams (currentStream);

          //Handle any random numbers in the station managers.
          Ptr<WifiRemoteStationManager> manager = wifi->GetRemoteStationManager ();
          Ptr<MinstrelWifiManager> minstrel = DynamicCast<MinstrelWifiManager> (manager);
          if (minstrel)
            {
              currentStream += minstrel->AssignStreams (currentStream);
            }

          //Handle any random numbers in the MAC objects.
          Ptr<WifiMac> mac = wifi->GetMac ();
          Ptr<RegularWifiMac> rmac = DynamicCast<RegularWifiMac> (mac);
          if (rmac)
            {
              PointerValue ptr;
              rmac->GetAttribute ("DcaTxop", ptr);
              Ptr<DcaTxop> dcaTxop = ptr.Get<DcaTxop> ();
              currentStream += dcaTxop->AssignStreams (currentStream);

              rmac->GetAttribute ("VO_EdcaTxopN", ptr);
              Ptr<EdcaTxopN> vo_edcaTxopN = ptr.Get<EdcaTxopN> ();
              currentStream += vo_edcaTxopN->AssignStreams (currentStream);

              rmac->GetAttribute ("VI_EdcaTxopN", ptr);
              Ptr<EdcaTxopN> vi_edcaTxopN = ptr.Get<EdcaTxopN> ();
              currentStream += vi_edcaTxopN->AssignStreams (currentStream);

              rmac->GetAttribute ("BE_EdcaTxopN", ptr);
              Ptr<EdcaTxopN> be_edcaTxopN = ptr.Get<EdcaTxopN> ();
              currentStream += be_edcaTxopN->AssignStreams (currentStream);

              rmac->GetAttribute ("BK_EdcaTxopN", ptr);
              Ptr<EdcaTxopN> bk_edcaTxopN = ptr.Get<EdcaTxopN> ();
              currentStream += bk_edcaTxopN->AssignStreams (currentStream);

              //if an AP, handle any beacon jitter
              Ptr<ApWifiMac> apmac = DynamicCast<ApWifiMac> (rmac);
              if (apmac)
                {
                  currentStream += apmac->AssignStreams (currentStream);
                }
            }
        }
    }
  return (currentStream - stream);
}*/

int64_t
WifiApHelper::AssignStreams (NetDeviceContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  Ptr<NetDevice> netDevice;
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      netDevice = (*i);
      Ptr<WifiNetDevice> wifi = DynamicCast<WifiNetDevice> (netDevice);
      if (wifi)
        {
          //Handle any random numbers in the PHY objects.
          currentStream += wifi->GetPhy ()->AssignStreams (currentStream);

          //Handle any random numbers in the station managers.
          Ptr<WifiRemoteStationManager> manager = wifi->GetRemoteStationManager ();
          Ptr<MinstrelWifiManager> minstrel = DynamicCast<MinstrelWifiManager> (manager);
          if (minstrel)
            {
              currentStream += minstrel->AssignStreams (currentStream);
            }

          Ptr<MinstrelHtWifiManager> minstrelHt = DynamicCast<MinstrelHtWifiManager> (manager);
          if (minstrelHt)
            {
              currentStream += minstrelHt->AssignStreams (currentStream);
            }

          //Handle any random numbers in the MAC objects.
          Ptr<WifiMac> mac = wifi->GetMac ();
          Ptr<RegularWifiMac> rmac = DynamicCast<RegularWifiMac> (mac);
          if (rmac)
            {
              PointerValue ptr;
              rmac->GetAttribute ("Txop", ptr);
              Ptr<Txop> txop = ptr.Get<Txop> ();
              currentStream += txop->AssignStreams (currentStream);

              rmac->GetAttribute ("VO_Txop", ptr);
              Ptr<QosTxop> vo_txop = ptr.Get<QosTxop> ();
              currentStream += vo_txop->AssignStreams (currentStream);

              rmac->GetAttribute ("VI_Txop", ptr);
              Ptr<QosTxop> vi_txop = ptr.Get<QosTxop> ();
              currentStream += vi_txop->AssignStreams (currentStream);

              rmac->GetAttribute ("BE_Txop", ptr);
              Ptr<QosTxop> be_txop = ptr.Get<QosTxop> ();
              currentStream += be_txop->AssignStreams (currentStream);

              rmac->GetAttribute ("BK_Txop", ptr);
              Ptr<QosTxop> bk_txop = ptr.Get<QosTxop> ();
              currentStream += bk_txop->AssignStreams (currentStream);

              //if an AP, handle any beacon jitter
              Ptr<ApWifiMac> apmac = DynamicCast<ApWifiMac> (rmac);
              if (apmac)
                {
                  currentStream += apmac->AssignStreams (currentStream);
                }
            }
        }
    }
  return (currentStream - stream);
}

} //namespace ns3
