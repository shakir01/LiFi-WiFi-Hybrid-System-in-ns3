#include <math.h>
#include <cmath>
#include "lifi-snr.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LiFiSnr");	// define a log component with the name "VlcSnr"

NS_OBJECT_ENSURE_REGISTERED (LiFiSnr);// register VlcSnr class with the TypeId system

TypeId LiFiSnr::GetTypeId(void)	// returns meta-information about VlcSnr class 
		{ 	// including parent class, group name, constructor, and attributes
	static TypeId tid =
			TypeId("ns3::LiFiSnr").SetParent<Object>().SetGroupName("Network").AddConstructor<
					LiFiSnr>().AddAttribute("PowerReceived",
					"average received optical signal power", DoubleValue(0),
					MakeDoubleAccessor(&LiFiSnr::Pr),
					MakeDoubleChecker<double>()).AddAttribute("SNR",
					"signal-to-noise-ratio", DoubleValue(0),
					MakeDoubleAccessor(&LiFiSnr::SNR),
					MakeDoubleChecker<double>()).AddAttribute("LowerWavelength",
					"lower bound WaveLength", IntegerValue(0),
					MakeIntegerAccessor(&LiFiSnr::wavelength_lower),
					MakeIntegerChecker<int>()).AddAttribute("UpperWavelength",
					"upper bound WaveLength", IntegerValue(0),
					MakeIntegerAccessor(&LiFiSnr::wavelength_upper),
					MakeIntegerChecker<int>()).AddAttribute("Temperature",
					"Blackbody temp of LED", DoubleValue(0),
					MakeDoubleAccessor(&LiFiSnr::temp),
					MakeDoubleChecker<double>());
	return tid;
}

//Values of the Standard Luminocity Functions
double LiFiSnr::V_lambda[] = { 0.000039, 0.000120, 0.000396, 0.001210, 0.004000,
		0.011600, 0.023000, 0.038000, 0.060000, 0.090980, 0.139020, 0.208020,
		0.323000, 0.503000, 0.710000, 0.862000, 0.954000, 0.994950, 0.995000,
		0.952000, 0.870000, 0.757000, 0.631000, 0.503000, 0.381000, 0.265000,
		0.175000, 0.107000, 0.061000, 0.032000, 0.017000, 0.008210, 0.004102,
		0.002091, 0.001047, 0.000520, 0.000249, 0.000120, 0.000060, 0.000030 };

//General values for Respositvity 
double LiFiSnr::Response[] = { 0.150, 0.160, 0.170, 0.190, 0.200, 0.220, 0.230,
		0.240, 0.250, 0.260, 0.270, 0.280, 0.300, 0.320, 0.330, 0.350, 0.360,
		0.370, 0.375, 0.380, 0.390, 0.400, 0.415, 0.420, 0.430, 0.440, 0.450,
		0.460, 0.470, 0.475, 0.480, 0.485, 0.490, 0.495, 0.500, 0.505, 0.510,
		0.520, 0.526, 0.532 };

// constructor
LiFiSnr::LiFiSnr() {
	NS_LOG_FUNCTION(this);
	noise_var = 0;   // total noise variance
	Pr = 0;			// average received optical signal power
	res = 0;  		// responsitivity of receiver
	SNR = 0;  		// signal-to-noise ratio
	B = 0;
	wavelength_lower = 0;	// lower bound WaveLength [nm]
	wavelength_upper = 0;	// upper bound Wavelength [nm]
	temp = 0;  			// Blackbody temp of LED
}

// destructor
LiFiSnr::~LiFiSnr() {
	NS_LOG_FUNCTION(this);
}

// sets upper and lower bound wavelength 
void LiFiSnr::SetWavelength(int lower, int upper) {
	NS_LOG_FUNCTION(this << lower << upper);
	wavelength_lower = lower;
	wavelength_upper = upper;
}

// sets the blackbody temperature of LED
void LiFiSnr::SetTemperature(double t) {
	NS_LOG_FUNCTION(this << t);
	temp = t;
}

double LiFiSnr::GetTemperature() {
	NS_LOG_FUNCTION(this);
	return this->temp;
}

// sets the average received optical signal power
void LiFiSnr::SetReceivedPower(double p) {
	NS_LOG_FUNCTION(this << p);
	Pr = p;
}

/*// sets the average received optical signal power
double VlcSnr::GetReceivedPower() {
	return Pr;
}*/



// CalculateNoiseVar method calculates the noise variance using the following parameters:
// Ib: background current [A]
// I2: noise-bandwidth factor
// I3: 
// A: photodetector Area [m^2]
// B: noise bandwidth
// Tk: absolute temperature [K]
// Cpd: fixed capacitance of photodetector per unit area [F/cm^2]
// Gol: open-loop voltage gain
// gamma: FET channel noise factor
// gm: FET transconductance [s]
void LiFiSnr::CalculateNoiseVar(double A) {
	NS_LOG_FUNCTION(this);
B = 5e+6;
	//res = IntegralRes() / IntegralPlanck();
	res = 0.2;//0.325-BER PAM4 Goodput0.160//OOK GOodpt=0.130 BER 0.149 ////VPPM 6 Goodput=0.148 BER 0.198////VPPM8 Goodput=0.160 BER=0.340--0.333 
	static const double q = 1.602176487e-19;	//electronic charge [Coulombs]
	static const double k = 1.38064852e-23;	//Boltzmann constant [m^2 kg s^-2 K^-1]
	static const double I2 = 0.5620; //noise bandwidth factor
	static double I3 = 0.0868; //noise bandwidth factor
	static double Ib = 5100e-6 ;//std::pow((double)5100.0, -6.0);/*5100.134e-6  0.005100*/; //photocurrent due to background radiation [microA]
	static double Gol = 10; //open-loop voltage gain
	static double Cpd = 112e-10;/*1.12e-10*/; //fixed capacitance of photodetector per unit area [pF/m^2]
	static double gm = /*0.030*/30e-3; //FET transconductance [mS]
	static double gamma = 1.5; //FET channel noise factor

	double shot_var, thermal_var;
//std::cout<<"A: "<<A<<"Recieved Power For Noise: "<<Pr<<" BandWidth Factor: "<<B<<" Responsivity: "<<res<<std::endl;

	// shot noise variance
	shot_var = (2 * q * res * Pr * B) + (2 * q * Ib * I2 * B);

	// thermal noise variance
	thermal_var = (8 * M_PI * k * temp * Cpd * A * I2 * (std::pow(B, 2)) / Gol)
			+ (16 * (std::pow(M_PI, 2)) * k * temp * gamma * (std::pow(Cpd, 2))
					* (std::pow(A, 2)) * I3 * (std::pow(B, 3)) / gm);

	noise_var = shot_var + thermal_var;
}

// caluclates the SNR value using received power, responsivity and noise variance
void LiFiSnr::CalculateSNR() {

	NS_LOG_FUNCTION(this);
	if (noise_var != 0)
{
//Pr =  1.9428e-06/*1.78935e-06*/;
 //noise_var = 4.59289e-15;
SNR = std::pow((Pr * res), 2) / noise_var;
//std::cout<<"SNR "<<SNR<<" res "<<res<<" noise_var "<<noise_var<<std::endl;
}
}


// returns the signal-to-noise ratio (SNR)
double LiFiSnr::GetSNR() {
	NS_LOG_FUNCTION(this);
	CalculateSNR();
	return SNR;
}

// Definite integral of the Spectral Radiance(wavelength, temperature) d(wavelength)
double LiFiSnr::IntegralPlanck() {
	NS_LOG_FUNCTION(this);
	double integral = 0;

	while (wavelength_lower <= wavelength_upper) {
		integral += SpectralRadiance(wavelength_lower, temp) * 10e-9;
		wavelength_lower += 10;
	}
	return integral;
}

// Definite integral of the Response(wavelength)*Spectral Radiance(wavelength, temperature) d(wavelength)
double LiFiSnr::IntegralRes() {
	NS_LOG_FUNCTION(this);
	double integral = 0;
	while (wavelength_lower <= wavelength_upper) {
		integral += Response[(wavelength_lower - /*380*/wavelength_upper) / 10]
				* SpectralRadiance(wavelength_lower, temp) * 10e-9;
		wavelength_lower += 10;
	}
	return integral;
}

void LiFiSnr::SetElectricNoiseBandWidth(double b) {	// sets the noise bandwidth
	NS_LOG_FUNCTION(this);
	B = b;
}

double LiFiSnr::GetNoiseBandwidth() {			//return the noise bandwidth
	NS_LOG_FUNCTION(this);
	return B;
}

// Calculates Spectral Radiance based on wavelength and Blackbody temp of LED
double LiFiSnr::SpectralRadiance(int wavelength, double temperature) {
	NS_LOG_FUNCTION(this);
	double spectral_rad;
	double h = 6.62607004;	//Planck's constant
	double c = 299792458;	//speed of light
	double k = 1.3806488e-23;	//Boltzmann constant
	double waveLength = wavelength * 1e-9; //nm
	spectral_rad =
			15 * (std::pow((h * c) / (M_PI * k * temperature), 4))
					/ ((std::pow(waveLength, 5))
							* ((std::exp(
									(h * c) / (waveLength * k * temperature)))
									- 1));
	return spectral_rad;
}

} // namespace ns3

