#ifndef COMMONS_H
#define COMMONS_H
#include "ns3/data-rate.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/ptr.h"
#include "ns3/pointer.h"
#include "ns3/mobility-model.h"
#include "ns3/point-to-point-net-device.h"
#include  <map>

#include <math.h>

namespace ns3 
{
struct RWPData
{
   Vector initialPoint;
   Vector detinaionPoint;
   double ueSpeed;
   double walkAngle;
   double elapsedTime;
   std::vector<double> polarAnglesVector;
};

struct ChannelStats
{
   uint32_t 		  u_id;
   double 		    simTime;
   double         snr;
   double         errRate;
   double         loss;
   double         throughput;
};

struct Connection
{
   uint32_t 		  u_id;
   Ipv4Address 	  destinationIpAddress;
   uint16_t         destinationPort;
};

struct LifiPhyParameters
{
   double         semiAngle;
   double         photoDetectorArea;
   double         filterGain;
   double         FOVAngle;
   double         refractiveIndex;
   double         dataRateInMBPS;
   double         beta;
   double         dutyCycle;
   int            modulation;
   bool           isVPPM;
};

struct TdmaApMac
{
   DataRate       dateRate;
   Time           slotTime;
   Time           guardTime;
   Time           interFrameTime;
   Time           beaconInterval;
};

struct TdmaUserMac
{
   DataRate       dateRate;
   Time           slotTime;
   Time           guardTime;
   Time           interFrameTime;
   Time           beaconInterval;
   uint32_t       maxMissedBeacons;
};

}
#endif