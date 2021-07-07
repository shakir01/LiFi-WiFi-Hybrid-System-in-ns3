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

#ifndef WIFI_AP_HELPER_H
#define WIFI_AP_HELPER_H

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-phy-standard.h"
#include "ns3/trace-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/wifi-helper.h"

namespace ns3 {

class WifiPhy;
class Node;
class WifiApNetDevice;
class WifiHelper;
/**
 * \brief helps to create WifiNetDevice objects
 *
 * This class can help to create a large set of similar
 * WifiNetDevice objects and to configure a large set of
 * their attributes during creation.
 */
class WifiApHelper : public WifiHelper
{
public:

  /**
   * Create a Wifi helper in an empty state: all its parameters
   * must be set before calling ns3::WifiHelper::Install
   *
   * The default state is defined as being an Adhoc MAC layer with an ARF rate control algorithm
   * and both objects using their default attribute values. 
   * By default, configure MAC and PHY for 802.11a.
   */
  WifiApHelper ();

  
  /**
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param c the set of nodes on which a wifi device must be created
   * \returns a device container which contains all the devices created by this method.
   */
  virtual NetDeviceContainer Install (const WifiPhyHelper &phy,
                                     const WifiMacHelper &mac, NodeContainer c) const;
    NetDeviceContainer
Install (const WifiPhyHelper &phyHelper,
                     const WifiMacHelper &macHelper,
                     NodeContainer::Iterator first,
                     NodeContainer::Iterator last) const;
  /**
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param node the node on which a wifi device must be created
   * \returns a device container which contains all the devices created by this method.
   */
  virtual NetDeviceContainer Install (const WifiPhyHelper &phy,
                                      const WifiMacHelper &mac, Ptr<Node> node) const;
  /**
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param nodeName the name of node on which a wifi device must be created
   * \returns a device container which contains all the devices created by this method.
   */
  virtual NetDeviceContainer Install (const WifiPhyHelper &phy,
                                      const WifiMacHelper &mac, std::string nodeName) const;
int64_t
AssignStreams (NetDeviceContainer c, int64_t stream);

};

} //namespace ns3

#endif /* WIFI_AP_HELPER_H */
