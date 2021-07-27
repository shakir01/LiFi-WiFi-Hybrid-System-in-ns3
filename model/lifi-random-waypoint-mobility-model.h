
#ifndef LIFI_RANDOM_WAYPOINT_MOBILITY_MODEL_H
#define LIFI_RANDOM_WAYPOINT_MOBILITY_MODEL_H 
#include "ns3/random-waypoint-mobility-model.h"
#include "ns3/constant-velocity-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/position-allocator.h"
//#include  <boost/random.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include "ns3/random-variable-stream.h"
#include "ns3/ptr.h"
#include "ns3/vector.h"
#include "ns3/enum.h"
#include "ns3/double.h"
#include <iostream>
#include <fstream>
#include "ns3/random-variable-stream.h"
#include "ns3/commons.h"

namespace ns3 {

class LiFiRandomWaypointMobilityModel : public MobilityModel
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  LiFiRandomWaypointMobilityModel();

  static TypeId GetTypeId (void);
void BeginWalk (Vector initialPosition, Vector prevPoint, Vector destination, double currMove, double walkAngle, double elevaAngle);
//void BeginWalk (Vector initialPosition, Vector prevPoint, double test);

RWPData* GetMobilityData();
void SetSpeedUE (double);


protected:
  virtual void DoInitialize (void);
private:
  /**
   * Get next position, begin moving towards it, schedule future pause event
   */
 void InitiateWalk1 (void);

  /**
   * Begin current pause event, schedule future walk event
   */
  void DoInitializePrivate (void);

  void InitiateWalk (void);

  double GeneratePolarAngleGain (double previousGain) ;

  virtual Vector DoGetPosition (void) const;
  virtual void DoSetPosition (const Vector &position);
  virtual Vector DoGetVelocity (void) const;
  virtual int64_t DoAssignStreams (int64_t);
  double CalculateIncidenceAngle();


  void SetSpeed (double speed);
  double GetSpeed ( );

  void SetPolarAngleCoherenceTime (double coherenceTime);
  double GetPolarAngleCoherenceTime ( );

  void SetMeanOfRandomProcess (double meanRP );
  double GetMeanOfRandomProcess ( );

  void SetVarianceOfRP (double varRP);
  double GetVarianceOfRP ( );
  double GenerateNoiseSamples(double mean, double variance);

  ConstantVelocityHelper m_helper; //!< helper for velocity computations
  Ptr<PositionAllocator> m_position; //!< pointer to position allocator
  Ptr<RandomVariableStream> m_speed; //!< random variable to generate speeds
  Ptr<RandomVariableStream> m_pause; //!< random variable to generate pauses
  EventId m_event; //!< event ID of next scheduled event

  double m_speedUE;
  double m_coherenceTime;
  double m_meanofRP;
  double m_varianceofRP;
  double d_Consec_Points;
  int index ;
  std::vector <RWPData*> m_RWPDataVector;
  boost::random::mt19937 m_engine;
  double elapsedTime;
  ns3::Ptr<NormalRandomVariable> m_NormalRandomVariable;
  bool start;
};
} // namespace ns3

#endif /* RANDOM_WAYPOINT_MOBILITY_MODEL_H */
