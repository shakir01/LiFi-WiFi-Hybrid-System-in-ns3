#include "received-data-model.h"

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (ReceivedDataModel);
NS_LOG_COMPONENT_DEFINE ("ReceivedDataModel");

TypeId ReceivedDataModel::GetTypeId()	// returns meta-information about VLC_ErrorModel class
		{ 	// including parent class, group name, constructor, and attributes
	static ns3::TypeId tid = ns3::TypeId("ReceivedDataModel").SetParent<Object>().AddConstructor<ReceivedDataModel>();
	return tid;

}

ReceivedDataModel::ReceivedDataModel ()
{
    receivedPacket = 0;
    snr            = 0;
    ber		   = 0;
    receivedPower  = 0 ;
    timeReceived   = 0;
    isCorrupted    = false;
}

ReceivedDataModel::~ReceivedDataModel ()
{

}

void ReceivedDataModel::SetReceivedPacket (Ptr<ns3::Packet> p)
{
     receivedPacket = p;
}

void ReceivedDataModel::SetSNR (double d)
{
     snr = d;
}

void ReceivedDataModel::SetBER (double br)
{
     ber = br;
}

void ReceivedDataModel::SetReceivedPower (double pr)
{
    receivedPower = pr;
}

void ReceivedDataModel::SetTimeReceived (double time)
{
   timeReceived = time;
}

void ReceivedDataModel::SetIsCorrupted (bool corrupt)
{
    isCorrupted = false;
}

Ptr<ns3::Packet> ReceivedDataModel::GetReceivedPacket ()
{
  return receivedPacket;
}

bool ReceivedDataModel::GetSNR ()
{
  return snr;
}

double ReceivedDataModel::GetBER ()
{
  return ber;
}

double ReceivedDataModel::GetReceivedPower()
{
  return receivedPower;
}

double ReceivedDataModel::GetTimeReceived ()
{
  return timeReceived;
}

bool ReceivedDataModel::GetIsCorrupted ()
{
  return isCorrupted;
}
}


