/*std::cout<<"tx "<<tx<<" userPoistionData.currentPoint "<<userPoistionData.currentPoint<<" distance "<<distance<<std::endl;
	double xdist = std::pow((tx.x-userPoistionData.currentPoint.x),2);
	double ydist = std::pow((tx.y-userPoistionData.currentPoint.y),2);
	double a = std::sqrt((xdist + ydist)/distance)*cos((userPoistionData.angleOfDirection) -atan((tx.y-userPoistionData.currentPoint.y)/(tx.x-userPoistionData.currentPoint.x)));
	double b =  (tx.z-userPoistionData.currentPoint.z)/distance;
        double incidenceGain = a*sin(userPoistionData.polarAngleGain) + b*cos(userPoistionData.polarAngleGain);
*/

#include "ns3/lifi-propagation-loss-model.h"
#include <cmath>
#include "ns3/lifi-rx-net-device.h"
#include "ns3/lifi-tx-net-device.h"
#include "ns3/lifi-random-waypoint-mobility-model.h"
#include "ns3/lifi-constant-velocity-mobility-model.h"
#include "ns3/random-variable-stream.h"

#define _USE_MATH_DEFINES
#include "math.h"
namespace ns3 {

LiFiPropagationLossModel::LiFiPropagationLossModel()
{
	m_TxPowerMAX = 0;
	m_TxPower = 0;
	m_FilterGain = 0;
	m_ConcentratorGain = 0;
	m_RxPower = 0;
    m_LoSChannelGain = 0;
	m_RXGain = 0;
	m_TXGain = 0;
    m_TxRxDistance = 0;
	m_TxNormalVector.push_back(0);
	m_TxNormalVector.push_back(0);
	m_TxNormalVector.push_back(1);
	min =0;
    max =180;
}

void LiFiPropagationLossModel::SetTxPower(double watt)
{
	this->m_TxPower = watt;
}
void LiFiPropagationLossModel::SetTxPowerMAX(double MAX)
{
	this->m_TxPowerMAX = MAX;
}

double LiFiPropagationLossModel::GetTxPower()
{
	return this->m_TxPower;
}

double LiFiPropagationLossModel::GetTxPowerMAX()
{
	return this->m_TxPowerMAX;
}

void LiFiPropagationLossModel::SetFilterGain(double gain)
{
	this->m_FilterGain = gain;
}

double LiFiPropagationLossModel::GetFilterGain()
{
	return this->m_FilterGain;
}

void LiFiPropagationLossModel::SetConcentratorGain(double cGain)
{
	this->m_ConcentratorGain = cGain;
}

double LiFiPropagationLossModel::GetConcentratorGain() {
	return this->m_ConcentratorGain;
}

double LiFiPropagationLossModel::DoCalcRxPower(double txPowerDbm, ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const
{
	//std::cout<<"double LiFiPropagationLossModel::DoCalcRxPower(double txPowerDbm, ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const "<<std::endl;
	double rxGain = 0;
	double pRX;
	Ptr<ns3::LiFiConstantVelocityMobilityModel>  lifiConstantVelModel  = ns3::DynamicCast < LiFiConstantVelocityMobilityModel>  ( bRX );
	Ptr<ns3::LiFiRandomWaypointMobilityModel>  lifiRandomWaypointModel  = ns3::DynamicCast < LiFiRandomWaypointMobilityModel>  ( bRX );
	double txGain = CalculateTxGain(aTX,bRX);

    if (lifiConstantVelModel != 0)
    {
		rxGain = CalculateRxGainWithHeight(aTX,bRX);
		pRX = 20.5 * rxGain * txGain* this->m_FilterGain * this->m_ConcentratorGain * this->m_Area;
		//std::cout<<"double LiFiPropagationLossModel::DoCalcRxPower rxGain "<<pRX<<std::endl;
	}
	else if(lifiRandomWaypointModel != 0)
	{
		//Vector tx = aTX->GetPosition();
	    //Vector rx = bRX->GetPosition();
		rxGain = CalculateRxGain(aTX,bRX);
		//std::cout<<"Simulation time:"<<Simulator::Now().GetSeconds()<<" tx "<<tx<<" rx "<<rx<<" rx_gain "<<rxGain<<" tx gain "<<txGain<<std::endl;
		pRX = 11.0* rxGain * txGain* this->m_FilterGain * this->m_ConcentratorGain * this->m_Area;
		//std::cout<<"double LiFiPropagationLossModel::DoCalcRxPower rxGain "<<rxGain<<" txGain "<<txGain<<" pRX "<<pRX<<std::endl;
	}
	else 
	{
	//	Vector tx = aTX->GetPosition();
	//Vector rx = bRX->GetPosition();
		pRX = 11.5*0.47*txGain* this->m_FilterGain * this->m_ConcentratorGain * this->m_Area;
	}

	double distance = GetDistance(aTX, bRX);
	pRX = (double)pRX /std::pow(distance, 2);
	//std::cout<<"Power pRX "<<pRX<<std::endl;
	return pRX;
}

Vector LiFiPropagationLossModel::GetCurrentTxPostion()
{
  	return currentTxPostion;
}

Vector LiFiPropagationLossModel::GetCurrentRxPostion()
{
  	return currentRxPostion;
}

double LiFiPropagationLossModel::GetPolarAngle ()
{
	return m_polarAngle;
}

double LiFiPropagationLossModel::GetRxGain ()
{
 	return m_RXGain;
}

double LiFiPropagationLossModel::GetLoSChannelGain()
{
        return m_LoSChannelGain;
}

double LiFiPropagationLossModel::GetRxPower()
{
	return this->m_RxPower;
}

void LiFiPropagationLossModel::SetRXPower(double wattTX,
		ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX)
{
	this->m_RxPower = this->DoCalcRxPower(wattTX, aTX, bRX);
}

int64_t LiFiPropagationLossModel::DoAssignStreams(int64_t stream)
{
	return stream;
}

void LiFiPropagationLossModel::SetUpTxandRx (ns3::Ptr<LiFiTxNetDevice> liFiTxNetDevice,ns3::Ptr<LiFiRxNetDevice> liFiRxNetDevice)
{
    m_LifiTxNetDevice = liFiTxNetDevice;
    m_LifiRxNetDevice = liFiRxNetDevice;
}

double LiFiPropagationLossModel::GetDistance(ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX) const
{ 	
	double dist = 0;
	Vector tx = aTX->GetPosition();
	Vector rx = bRX->GetPosition();

	dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2) + std::pow((tx.z - rx.z), 2);
	dist = std::sqrt(dist);
	return dist;
}

double LiFiPropagationLossModel::GetDistance(Vector tx, Vector rx) const
{
	double dist = 0;
	dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2) + std::pow((tx.z - rx.z), 2);
	dist = std::sqrt(dist);
	return dist;
}

double LiFiPropagationLossModel::CalculateTxGain(ns3::Ptr<ns3::MobilityModel> aTX, ns3::Ptr<ns3::MobilityModel> bRX)
const {
	Vector tx = aTX->GetPosition();
	Vector rx = bRX->GetPosition();
	double height = std::sqrt(std::abs(tx.z - rx.z));
	double distance = GetDistance(tx,rx);
	double txGain= height/distance;
//std::cout<<"CalculateTxGain tx "<<tx<<" rx "<<rx<<" txGin "<<txGain<<" height "<<height<<" txGain "<<txGain<<std::endl;
	return txGain;
}


double LiFiPropagationLossModel::GetDistanceConsiderHeight(ns3::Ptr<ns3::MobilityModel> aTX,
		ns3::Ptr<ns3::MobilityModel> bRX) const
{
	double dist = 0;
	Vector tx = aTX->GetPosition();
	Vector rx = bRX->GetPosition();

	dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2) + std::pow((3.8), 2);
	dist = std::sqrt(dist);

	return dist;
}

double LiFiPropagationLossModel::CalculateRxGainWithHeight(ns3::Ptr<ns3::MobilityModel> aTX,
		ns3::Ptr<ns3::MobilityModel> bRX )  const
{
  //Ptr<ns3::MobilityModel> lifimodel1 = m_LifiRxNetDevice->GetNode()->GetObject<ns3::MobilityModel>();

  Vector tx = aTX->GetPosition();
  Vector rx = bRX->GetPosition();

   double gamma = 0;
   double beta = (45.0 * M_PI)/ 180.0;
   double alpha =0;
  // Create the RNG with the specified range.
  
   Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
   x->SetAttribute ("Min", DoubleValue (min));
   x->SetAttribute ("Max", DoubleValue (max));
   alpha = x->GetValue ();
   alpha = (alpha * M_PI)/ 380.0;
   double dist = GetDistance(aTX,bRX);
   double rxGain = 0;
   rxGain =  (1/dist)*((tx.x- rx.x)*(sin(alpha)*sin(beta)*cos(gamma) + cos(alpha)*sin(beta))+(tx.y-rx.y)*(sin(alpha)*sin(gamma) - cos(alpha)*sin(beta)*cos(gamma))+ 3.8*cos(beta)*cos(gamma));
	 double aoi_d = (acos(rxGain)*180/M_PI);
	 

  if (aoi_d > 90)
  {

   return  0;
  }

return rxGain;
}

double LiFiPropagationLossModel::CalculateRxGain(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX )  const
{
  Ptr<ns3::LiFiRandomWaypointMobilityModel> lifimodel1 = ns3::DynamicCast < ns3::LiFiRandomWaypointMobilityModel>  (bRX) ;
  RWPData* rwpData = lifimodel1->GetMobilityData();
  Vector tx = aTX->GetPosition();
  Vector rx;
  double polarAngle;
  if (rwpData != NULL)
  {
	  	//  std::cout<<"double LiFiPropagationLossModel::CalculateRxGain(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX )  rwpData != NULL "<<std::endl;
	  rx = rwpData->detinaionPoint;
	  polarAngle = rwpData->polarAngle;
  }
  else
  {
	 // std::cout<<"std::cout<<"double LiFiPropagationLossModel::CalculateRxGain(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX )  rwpData != NULL "<<std::endl;
	
	  return 0.91;
  }
  
  double l_1= CalculateLambda1(tx,rx); 
  double l_2 = CalculateLambda2(tx,rx);
  double rx_gain = std::sqrt(std::pow(l_1,2) + std::pow(l_2,2)) * cos(polarAngle - atan (l_1/l_2));
  double aoi_d = (acos(rx_gain)*180/M_PI);

  //std::cout<<"At Simulation Time: "<<ns3::Simulator::Now().GetSeconds()<<" CalculateRxGain rx_gain "<<rx_gain<<" gain in degrees "<<aoi_d<<std::endl;
  if (aoi_d > 90)
  {
	return  0;
   }
  return rx_gain;
}

double LiFiPropagationLossModel::CalculateLambda1 (Vector apPosition, Vector rxPosition) const
{
	double r = std::sqrt(std::pow((rxPosition.x - apPosition.x),2) + std::pow((rxPosition.y - apPosition.y),2));
	double d = GetDistance(apPosition,rxPosition);	
	double azim = CalculateAzimuth(apPosition,rxPosition);
	double lambda1 =  (r/d)*cos(azim - atan((rxPosition.x - apPosition.x)/(rxPosition.y - apPosition.y)));
	return lambda1;
}

double LiFiPropagationLossModel::CalculateLambda2 (Vector apPosition, Vector rxPosition) const
{
	double h = std::abs(apPosition.z - rxPosition.z);
	double d = GetDistance(apPosition,rxPosition);	
	double lambda2 = h/d;
	return lambda2;
}

double LiFiPropagationLossModel::CalculateCoefficient_a(Vector apPosition, Vector rxPosition, double WalkAngle)  const
{
	double distance = GetDistance(apPosition,rxPosition);

	double walkAngle2 = CalculateAzimuth (apPosition,rxPosition);
	double a_x_part = ((apPosition.x - rxPosition.x)/distance)*cos(walkAngle2);
	double a_y_part = ((apPosition.y - rxPosition.y)/distance)*sin(walkAngle2);
	double a = -a_x_part - a_y_part;
	return a;
}

double LiFiPropagationLossModel::CalculateCoefficient_b(Vector apPosition, Vector rxPosition)  const
{
	double distance = GetDistance(apPosition,rxPosition);
	double b = ((apPosition.z - rxPosition.z)/distance);
	return b;
}

double LiFiPropagationLossModel::CalculateAzimuth (Vector apPosition,Vector rxPosition) const
{
	double dx_dist = (apPosition.x - rxPosition.x);
	double dy_dist = (apPosition.y - rxPosition.y);
	double walkAngle = 0;
	
if ((dy_dist > 0 && dx_dist > 0)) //first quardant
	{
		walkAngle = atan(dy_dist/dx_dist);
	}
	else if ((dy_dist > 0 && dx_dist < 0))//second quardant
	{
		walkAngle = (M_PI - atan(dy_dist/dx_dist));
	}
	else if (dy_dist < 0 && dx_dist < 0)//three quardant
	{
		walkAngle = (M_PI + atan(dy_dist/dx_dist));
	}
	else if (dy_dist < 0 && dx_dist > 0)//fourth quardant
	{
		walkAngle = (2*M_PI - atan(dy_dist/dx_dist));
	}
	
	return walkAngle;
}

void LiFiPropagationLossModel::SetTXGain(double txgain)
{
	this->m_TXGain = txgain;
}

double LiFiPropagationLossModel::GetTXGain()
{
	return this->m_TXGain;
}

void LiFiPropagationLossModel::SetRXGain(double rxgain)
{
	this->m_RXGain = rxgain;
}

double LiFiPropagationLossModel::GetRXGain()
{
	return this->m_RXGain;
}

void LiFiPropagationLossModel::SetArea(double a)
{
	this->m_Area = a;
}

double LiFiPropagationLossModel::GetArea()
{
	return this->m_Area;
}

LiFiPropagationLossModel::~LiFiPropagationLossModel()
{

}

} /* namespace vlc */
