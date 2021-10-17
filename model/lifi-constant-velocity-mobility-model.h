#ifndef LIFI_CONSTANT_VELOCITY_MOBILITY_MODEL_H
#define LIFI_CONSTANT_VELOCITY_MOBILITY_MODEL_H

#include <stdint.h>
#include "ns3/nstime.h"
#include "mobility-model.h"
#include "ns3/double.h"
#include "constant-velocity-helper.h"

namespace ns3 {

/**
 * \ingroup mobility
 *
 * \brief Mobility model for which the current speed does not change once it has been set and until it is set again explicitely to a new value.
 */
class LiFiConstantVelocityMobilityModel : public MobilityModel 
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Create position located at coordinates (0,0,0) with
   * speed (0,0,0).
   */
  LiFiConstantVelocityMobilityModel ();
  virtual ~LiFiConstantVelocityMobilityModel ();

  /**
   * \param speed the new speed to set.
   *
   * Set the current speed now to (dx,dy,dz)
   * Unit is meters/s
   */
  void SetVelocity (const Vector &speed);

  void SetAzimuth(double angle);
  double GetAzimuth(void);

  void SetElevation(double angle);
  double GetElevation(void);

private:
  double CalculatePolarAngle (void);

  virtual Vector DoGetPosition (void) const;
  virtual void DoSetPosition (const Vector &position);
  virtual Vector DoGetVelocity (void) const;
  ConstantVelocityHelper m_helper;  //!< helper object for this model
  double m_azimuth;    //Rotation (left/;; and Right)
  double m_elevation; //Rotation Angle};
};

} // namespace ns3

#endif /* CONSTANT_VELOCITY_POSITION */
