/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include <cmath>
#include "ns3/simulator.h"
#include "ns3/random-variable-stream.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "lifi-random-waypoint-mobility-model.h"
#include "ns3/position-allocator.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiRandomWaypointMobilityModel);

TypeId
LiFiRandomWaypointMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiRandomWaypointMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<LiFiRandomWaypointMobilityModel> ()
     .AddAttribute ("Speed",
                   "A random variable used to pick the speed of a random waypoint model.",
                   StringValue ("ns3::UniformRandomVariable[Min=0.2|Max=0.8]"),
                   MakePointerAccessor (&LiFiRandomWaypointMobilityModel::m_speed),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("Pause",
                   "A random variable used to pick the pause of a random waypoint model.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=2]"),
                   MakePointerAccessor (&LiFiRandomWaypointMobilityModel::m_pause),
                   MakePointerChecker<RandomVariableStream> ())
   .AddAttribute ("PositionAllocator",
                   "The position model used to pick a destination point.",
                   PointerValue (),
                   MakePointerAccessor (&LiFiRandomWaypointMobilityModel::m_position),
                   MakePointerChecker<PositionAllocator> ())
   .AddAttribute ("SpeedUE","The position model used to pick a destination point.",
                   DoubleValue(0.8),
                   MakeDoubleAccessor (&LiFiRandomWaypointMobilityModel::m_speedUE),
                   MakeDoubleChecker <double>())
    .AddAttribute ("CoherenceTimeofPolarAngle",
                   "The position model used to pick a destination point.",
                  DoubleValue(0.13),
                   MakeDoubleAccessor (&LiFiRandomWaypointMobilityModel::m_coherenceTime),
                   MakeDoubleChecker <double>())
   .AddAttribute ("MeanofRP",
                   "The position model used to pick a destination point.",
                   DoubleValue(0),
                   MakeDoubleAccessor (&LiFiRandomWaypointMobilityModel::m_meanofRP),
                   MakeDoubleChecker <double>())
   .AddAttribute ("VarianceofRP",
                   "The position model used to pick a destination point.",
                   DoubleValue(0),
                   MakeDoubleAccessor (&LiFiRandomWaypointMobilityModel::m_varianceofRP),
                   MakeDoubleChecker <double>());

  return tid;
}

LiFiRandomWaypointMobilityModel::LiFiRandomWaypointMobilityModel()
{

}

void LiFiRandomWaypointMobilityModel::SetSpeedUE(double speed)
{
      m_speedUE = speed;
}

void LiFiRandomWaypointMobilityModel::BeginWalk (Vector initialPosition, Vector prevPoint, Vector destination, double currMove, double walkAngle, double elevaAngle)
{
  m_helper.Update ();
  // std::cout<<"  std::cout<<m_RWPDataVector.size()-1 m_RWPDataVector.size()-1 "<<(m_RWPDataVector.size()-1)<<std::endl;
  //Vector m_current = m_helper.GetCurrentPosition ();
  NS_ASSERT_MSG (m_position, "No position allocator added before using this model");

  static bool firstbool =false;
  std::stringstream m_CSVlififileNamestream;
  m_CSVlififileNamestream<<"LiFi-Network/"<<"ORWPMobility";
  std::string m_CSVlififileName = m_CSVlififileNamestream.str();
 // std::cout<<"m_CSVlififileName "<<m_CSVlififileName<<std::endl;
  std::ofstream dat_lifi_file_out (m_CSVlififileName.c_str (), std::ios::app);

  if (firstbool == false)
  {
      dat_lifi_file_out<<"Time\t "<<" \t Curr_P.x "<<" \t Curr_P.y "<< std::endl;
      //dat_lifi_file_out<<Simulator::Now().GetSeconds()<<" \t "<<m_current.x<<" \t "<<m_current.y<< std::endl;
      firstbool = true;
  }
   // std::cout<<Simulator::Now().GetSeconds()<<" \t "<<prevPoint<<" \t "<<destination<<" \t "<<elevaAngle<<" currMove "<<currMove<<" elapsedTime "<<elapsedTime<<std::endl;


  if (currMove <= elapsedTime)
  {
          double cpoint_x = prevPoint.x  + m_speedUE*m_coherenceTime*cos(walkAngle);
          double cpoint_y = prevPoint.y  + m_speedUE*m_coherenceTime*sin(walkAngle);
          Vector vect;
          vect.x = cpoint_x;
          vect.y = cpoint_y;
          vect.z = prevPoint.z;

          double dx = (cpoint_x - prevPoint.x);
          double dy = (cpoint_y - prevPoint.y);
          double dz  =(vect.z - prevPoint.z);;
           double current_move = currMove + m_coherenceTime;
          double polarAngle = GeneratePolarAngleGain (elevaAngle);
        //  std::cout<<Simulator::Now().GetSeconds()<<" "<<cpoint_y<<" \t "<<vect<<" \t "<<prevPoint<<" \t "<<destination<<" \t "<<polarAngle<<std::endl;

          double k = m_speedUE / (std::sqrt (vect.x*vect.x+ vect.y*vect.y +vect.z*vect.z));
          m_helper.SetVelocity (Vector (k*dx, k*dy, k*dz));
         //std::cout<<"currMove <= elapsedTime: "<<Simulator::Now().GetSeconds()<<" \t "<<prevPoint<<" \t "<<destination<<" \t "<<polarAngle<<" currMove "<<current_move<<" elapsedTime "<<elapsedTime<<" walkAngle "<<walkAngle<<std::endl;
          dat_lifi_file_out<<Simulator::Now().GetSeconds()<<" \t "<<prevPoint.x<<" \t "<<prevPoint.y<< std::endl;

          RWPData* rwpData = new RWPData;
          rwpData->initialPoint = initialPosition;
          rwpData->detinaionPoint = destination;
          rwpData->ueSpeed = m_speedUE;
          rwpData->polarAngle = polarAngle;
          rwpData->walkAngle = walkAngle;
          m_helper.Unpause ();
          m_RWPDataVector.push_back(rwpData);
          Time travelDelay = Seconds (m_coherenceTime);
          //std::cout<<"Simulator::Now().GetSeconds() "<<Simulator::Now().GetSeconds()<<std::endl;
          m_event.Cancel ();
          m_event = Simulator::Schedule (travelDelay, &LiFiRandomWaypointMobilityModel::BeginWalk,this,initialPosition,vect,destination,current_move,walkAngle,polarAngle);
          NotifyCourseChange ();
  }
  else if (currMove != (elapsedTime-m_coherenceTime))
  {
          double polarAngle = GeneratePolarAngleGain (elevaAngle);
          RWPData* rwpData = new RWPData;
          rwpData->initialPoint = initialPosition;
          rwpData->detinaionPoint = destination;
          rwpData->ueSpeed = m_speedUE;
          rwpData->polarAngle = polarAngle;
          rwpData->walkAngle = walkAngle;
          m_RWPDataVector.push_back(rwpData);

          double dx = (destination.x - prevPoint.x);
          double dy = (destination.y - prevPoint.y);
          double dz  =(destination.z - prevPoint.z);;
          double d = std::sqrt (dx*dx + dy*dy + dz*dz);
          dat_lifi_file_out<<Simulator::Now().GetSeconds()<<" \t "<<prevPoint.x<<" \t "<<prevPoint.y<< std::endl;

          double k = m_speedUE / std::sqrt (dx*dx + dy*dy + dz*dz);
          m_helper.SetVelocity (Vector (k*dx, k*dy, k*dz));
          m_helper.SetPosition(destination);
         //  std::cout<<"currMove !=elapsedTime-: "<<Simulator::Now().GetSeconds()<<" Vector (k*dx, k*dy, k*dz) "<<Vector (k*dx, k*dy, k*dz)<<" destination "<<destination<<" prevPoint "<<prevPoint<<std::endl;
          Time travelDelay = Seconds (d/m_speedUE);
          m_event.Cancel ();
          m_event = Simulator::Schedule (travelDelay, &LiFiRandomWaypointMobilityModel::InitiateWalk, this);
         // std::cout<<"currMove !=elapsedTime-: "<<Simulator::Now().GetSeconds()<<" travelDelay "<<travelDelay.GetSeconds()<<" \t "<<prevPoint<<" \t "<<destination<<" \t "<<polarAngle<<" currMove "<<currMove<<" elapsedTime "<<elapsedTime<<std::endl;
          NotifyCourseChange ();
  }

}

double LiFiRandomWaypointMobilityModel::GeneratePolarAngleGain (double previousAngle)
{
   double c1 = std::pow (0.05, (0.001/m_coherenceTime));
   double c0 = (1-c1)*29.67;
   double polarGain = c0 + c1*previousAngle + m_NormalRandomVariable->GetValue();
  // std::cout<<"double LiFiRandomWaypointMobilityModel::GeneratePolarAngleGain (double previousAngle)  "<<polarGain<<std::endl;
   return polarGain;
}

double LiFiRandomWaypointMobilityModel::GenerateNoiseSamples(double mean, double variance)
{
  double noise_sample = 0;

  boost::function<double()> randn = boost::bind(boost::random::normal_distribution<>(0.0, std::sqrt(variance)),m_engine);
  noise_sample = randn();
    // std::cout<<"double LiFiRandomWaypointMobilityModel::GeneratePolarAngleGain (double previousAngle) noise_sample "<<noise_sample<<std::endl;

  return noise_sample;
}

void
LiFiRandomWaypointMobilityModel::DoInitialize (void)
{
  double c1 = std::pow (0.05, (0.01/m_coherenceTime));
  double variance = (1-pow(c1,2))*7.78;
  m_NormalRandomVariable = CreateObject<NormalRandomVariable>();
  m_NormalRandomVariable->SetAttribute("Mean",DoubleValue(0));
  m_NormalRandomVariable->SetAttribute("Variance",DoubleValue(variance));
  index = 0;
  size_t seed = 1234567890;
  boost::random::mt19937 engine(seed);
  m_engine = engine;
  DoInitializePrivate();
  MobilityModel::DoInitialize ();
}

void LiFiRandomWaypointMobilityModel::InitiateWalk (void)
{
  m_helper.Update ();
  m_helper.Pause ();
  Vector m_current    = m_helper.GetCurrentPosition ();
  NS_ASSERT_MSG (m_position, "No position allocator added before using this model");

  Vector destination  = m_position->GetNext ();
  double dx           = (destination.x - m_current.x);
  double dy           = (destination.y - m_current.y);
  double dz           = (destination.z - m_current.z);
  double d            = std::sqrt (dx*dx + dy*dy + dz*dz);

  double walkAngle    = atan2 (dy,dx);
  elapsedTime  = std::abs(d)/m_speedUE;
  double m_currentmov = 0;
  Time pause          = Seconds (m_pause->GetValue ());
  m_event             = Simulator::Schedule (pause, &LiFiRandomWaypointMobilityModel::BeginWalk,this,m_current,m_current,destination,m_currentmov,walkAngle,29.7);
}

void
LiFiRandomWaypointMobilityModel::DoInitializePrivate (void)
{
  m_event = Simulator::ScheduleNow (&LiFiRandomWaypointMobilityModel::InitiateWalk, this);
 // NotifyCourseChange ();
}

Vector
LiFiRandomWaypointMobilityModel::DoGetPosition (void) const
{
  m_helper.Update ();
  return m_helper.GetCurrentPosition ();
}
void
LiFiRandomWaypointMobilityModel::DoSetPosition (const Vector &position)
{
  m_helper.SetPosition (position);
  //std::cout<<"position x "<<position.x <<" position y "<<position.y<<std::endl;
  Simulator::Remove (m_event);
  m_event = Simulator::ScheduleNow (&LiFiRandomWaypointMobilityModel::DoInitializePrivate, this);
}
Vector
LiFiRandomWaypointMobilityModel::DoGetVelocity (void) const
{
  return m_helper.GetVelocity ();
}
int64_t
LiFiRandomWaypointMobilityModel::DoAssignStreams (int64_t stream)
{
  int64_t positionStreamsAllocated;
  m_speed->SetStream (stream);
  m_pause->SetStream (stream + 1);
  NS_ASSERT_MSG (m_position, "No position allocator added before using this model");
  positionStreamsAllocated = m_position->AssignStreams (stream + 2);
  return (2 + positionStreamsAllocated);
}

ns3::RWPData* LiFiRandomWaypointMobilityModel::GetMobilityData()
{
  if (m_RWPDataVector.size() > 0)
  {
    return m_RWPDataVector[m_RWPDataVector.size()-1];
  }

  return NULL;
}

void LiFiRandomWaypointMobilityModel::SetSpeed (double speed)
{
      m_speedUE = speed;
}

double LiFiRandomWaypointMobilityModel::GetSpeed ( )
{
        return m_speedUE;
}

void LiFiRandomWaypointMobilityModel::SetPolarAngleCoherenceTime (double coherenceTime)
{
        m_coherenceTime = coherenceTime;
}

double LiFiRandomWaypointMobilityModel::GetPolarAngleCoherenceTime ( )
{
        return m_coherenceTime;
}

void LiFiRandomWaypointMobilityModel::SetMeanOfRandomProcess (double meanRP )
{
        m_meanofRP = meanRP;
}

double LiFiRandomWaypointMobilityModel::GetMeanOfRandomProcess ( )
{
        return m_meanofRP;
}

void LiFiRandomWaypointMobilityModel::SetVarianceOfRP (double varRP )
{
        m_varianceofRP = varRP;
}

double LiFiRandomWaypointMobilityModel::GetVarianceOfRP ( )
{
        return m_varianceofRP;
}

} // namespace ns3
