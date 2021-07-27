#ifndef LIFI_PROPAGATION_LOSS_MODEL_H
#define LIFI_PROPAGATION_LOSS_MODEL_H

#include "ns3/point-to-point-channel.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/ptr.h"
#include "ns3/pointer.h"
#include "ns3/mobility-model.h"
#include "ns3/point-to-point-net-device.h"
#include  <map>
#include "ns3/commons.h"
#include <math.h>

namespace ns3 {
class LiFiRxNetDevice;
class LiFiTxNetDevice;

class LiFiPropagationLossModel: public ns3::PropagationLossModel 
{
public:
	/**
	 * \breif Get the type ID.
	 * \return the object TypeId
	 */
	//static ns3::TypeId GetTypeId(void);
	//Constructor
	LiFiPropagationLossModel();
	void SetTxPower(double watt);
	void SetTxPowerMAX(double MAX);

	double GetTxPower();
	double GetTxPowerMAX();

	void SetFilterGain(double gain);
	double GetFilterGain();

	void SetConcentratorGain(double cGain); //set concentration gain calculated in RX
	double GetConcentratorGain();

	void SetTXGain(double txgain);
	double GetTXGain();

	void SetRXGain(double rxgain);
	double GetRXGain();

	double GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,
			ns3::Ptr<ns3::MobilityModel> bRX) const;

	double GetRxPower();

	void SetArea(double a);
	double GetArea();

	void SetRXPower(double wattTX, ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX);

	double dotProduct(std::vector<double> v1, std::vector<double> v2) const;
	double magnitude(std::vector<double> v) const;

	double GetDistanceConsiderHeight(ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const;
	double CalculateTxGainWithHeight(double distance, double height)  const;
	double CalculateRxGainWithHeight(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX )  const;
	double CalculateRxGain(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX )  const;

    void SetUpTxandRx (ns3::Ptr<LiFiTxNetDevice>,ns3::Ptr<LiFiRxNetDevice>);
	Vector GetCurrentTxPostion();
	Vector GetCurrentRxPostion();

	double GetPolarAngle ();
	double GetRxGain ();
	double GetLoSChannelGain();

	double CalculateCoefficient_a(Vector apPosition, Vector rxPosition, double WalkAngle)  const;
	double CalculateCoefficient_b(Vector apPosition, Vector rxPosition)  const;
    double GetDistance(Vector tx, Vector rx) const;
	double CalculateAzimuth (Vector apPosition, Vector rxPosition) const;

	double CalculateLambda1 (Vector apPosition, Vector rxPosition) const;
	double CalculateLambda2 (Vector apPosition, Vector rxPosition) const;
    double CalculateTxGain(ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const;


	~LiFiPropagationLossModel();
private:
	/**
	 * \brief Copy constructor
	 *
	 * Defined and unimplemented to avoid misuse
	 */
	LiFiPropagationLossModel(const LiFiPropagationLossModel &);
	/**
	 * \brief Copy constructor
	 *
	 * Defined and unimplemented to avoid misuse
	 * \returns
	 */
	LiFiPropagationLossModel & operator =(const LiFiPropagationLossModel &);

	virtual double DoCalcRxPower(double txPowerDbm,
			ns3::Ptr<ns3::MobilityModel> aTX,
			ns3::Ptr<ns3::MobilityModel> bRX) const;

	virtual int64_t DoAssignStreams(int64_t stream);

	double CalculateTxGain (double distance, ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const; 

	std::vector<double> m_TxNormalVector;

	ns3::Ptr<LiFiTxNetDevice> m_LifiTxNetDevice;
	ns3::Ptr<LiFiRxNetDevice> m_LifiRxNetDevice;
	double m_TxPower;
	double m_TxPowerMAX;

	double m_FilterGain;
	double m_ConcentratorGain;
	double m_RxPower;
	double m_Area;
	double m_TXGain;

	double m_RXGain;
	double m_LoSChannelGain;
        double m_polarAngle;
        Vector currentTxPostion;
        Vector currentRxPostion;
 double min;
   double max;

	double m_TxRxDistance;

 /*struct RWPData2 
  {
     Vector previousPoint;
     Vector currentPoint;
     Vector detinaionPoint;
     double ueSpeed;
     double angleOfDirection;
     double polarAngleGain;  
  };*/

};

} /* namespace vlc */

#endif /* LIFIPROPAGATIONLOSS_H_ */
