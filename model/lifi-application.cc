#include "ns3/lifi-application.h"

namespace ns3 {

	LiFiApplication::LiFiApplication ()
	  : m_socket (0), 
	    m_peer (), 
	    m_packetSize (0), 
	    m_nPackets (0), 
	    m_dataRate (0), 
	    m_sendEvent (), 
	    m_running (false), 
	    m_packetsSent (0)
	{
	   m_node = 0;
           m_netdevice = 0;
	}


	LiFiApplication::~LiFiApplication()
	{
	  m_socket = 0;
          m_flowMonitor = 0;
	}

	void
	LiFiApplication::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
	{
	  m_socket = socket;
	  m_peer = address;
	  m_packetSize = packetSize;
	  m_nPackets = nPackets;
	  m_dataRate = dataRate;
	}

        void LiFiApplication::Setup (Ptr<Node > node, Ptr<ns3::NetDevice > lifiNetDevice, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
	{
	  m_netdevice =  lifiNetDevice;
	  m_node      =  node;
 	  Ptr<Socket> localSocket = Socket::CreateSocket (node, TcpSocketFactory::GetTypeId ());
	  localSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&LiFiApplication::CwndChange,this));
	  // Create a packet sink to receive these packets on n2...
	  localSocket->Bind (); 
	  m_netdevice->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&LiFiApplication::RxDrop,this));

	//InstallFlowMonitor ();
	  LiFiApplication::Setup (localSocket, address, packetSize,nPackets,dataRate);
	}

	ns3::Ptr<Socket> LiFiApplication::GetSocket (void)
	{
	   return m_socket;
	}

	void
	LiFiApplication::StartApplication (void)
	{
	  m_running = true;
	  m_packetsSent = 0;
	  m_socket->Bind ();
	  m_socket->Connect (m_peer);
	  SendPacket ();
	  transmissionTimeVector.push_back(Simulator::Now ().GetSeconds ());
	}

	void 
	LiFiApplication::StopApplication (void)
	{
	  m_running = false;

	  if (m_sendEvent.IsRunning ())
	    {
	      Simulator::Cancel (m_sendEvent);
	    }

	  if (m_socket)
	    {
	      m_socket->Close ();
	    }

	  if (m_flowMonitor != 0)
	     {
		/*std::string flowFile = "NameOfFileForNode ";
		int nodeId = m_node->GetId();
		std::ostringstream idstr;
		idstr<< nodeId;;
		std::string fileExtension = ".xml";
		std::string flowFileName = flowFile + idstr.str() + fileExtension;
		m_flowMonitor->SerializeToXmlFile(flowFileName, true, true);*/
	     }	
	}

	void 
	LiFiApplication::SendPacket (void)
	{
	  Ptr<Packet> packet = Create<Packet> (m_packetSize);
	  int sent = m_socket->Send (packet);
	  if (++m_packetsSent < m_nPackets)
	    {
	      ScheduleTx ();
	    }

	   if (sent != -1) 
	   {
		sentPacketsVector.push_back(packet);
	        transmissionTimeVector.push_back(Simulator::Now ().GetSeconds ());
	    }
	}

	void LiFiApplication::InstallFlowMonitor (void)
	{
	    m_flowMonitor = flowHelper.Install(m_node);
	}

	void 
	LiFiApplication::ScheduleTx (void)
	{
	  if (m_running)
	    {
	      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
	      m_sendEvent = Simulator::Schedule (tNext, &LiFiApplication::SendPacket, this);
	    }
	}

	 void LiFiApplication::CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
	{
  	// NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
	}

	 void  LiFiApplication::RxDrop (Ptr<const Packet> p)
	{
	  droppedPackets.push_back(droppedPackets.size() + 1);
if (m_node->GetId() !=4)
{

//std::cout<<"droppedPackets "<<droppedPackets.size()<<" m_node "<<m_node->GetId()<<std::endl;

}
	 // NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
	}

}// namespace ns3

