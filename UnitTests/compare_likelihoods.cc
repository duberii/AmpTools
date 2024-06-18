#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/ConfigurationInfo.h"
#include "IUAmpTools/AmpToolsInterface.h"
#include "DalitzDataIO/DalitzDataReader.h"
#include "DalitzAmp/BreitWigner.h"
#include "DalitzAmp/Constraint.h"

#include "IUAmpTools/report.h"
static const char* kModule = "fitAmplitudes";


using std::complex;
using namespace std;

int main( int argc, char* argv[] ){
  string cfgname(argv[1]);
  ConfigFileParser parser(cfgname);
  ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerNeg2LnLikContrib(Constraint());
  AmpToolsInterface::registerDataReader(DalitzDataReader());
  AmpToolsInterface ATI(cfgInfo);
  double neg2LL_before = ATI.likelihood();

  MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
  fitManager->setStrategy(1);

  fitManager->migradMinimization();

  if( fitManager->status() != 0 && fitManager->eMatrixStatus() != 3 ){
    report( WARNING, kModule ) << "Fit failed." << endl;
  }

  double neg2LL_after = ATI.likelihood();

  ATI.finalizeFit();
  cout << "Testing " << cfgname << " fit..." << endl;
  if (cfgname == "dalitz1.cfg") {
      cout << setprecision (15) << neg2LL_before - 3.079554803770344e+04 << endl;
      cout << setprecision (15) << neg2LL_after + 2.074778620651376e+03  << endl;
      //assert(abs(neg2LL_before - 3.079554803770344e+04) <= 1e-10);
      //assert(abs(neg2LL_after + 2.074778620651376e+03)<= 1e-10);
    } else if (cfgname == "dalitz2.cfg") {
      cout << setprecision (15) << neg2LL_before - 6.647422147638009e+04<< endl;
      cout << setprecision (15) << neg2LL_after + 5.548757456847361e+03 << endl;
      //assert(neg2LL_before == 66474.2);
      //assert(neg2LL_after == -5.54876e+03);
    } else if (cfgname == "dalitz3.cfg") {
      cout << setprecision (15) << neg2LL_before - 3.079554803770344e+04 << endl;
      cout << setprecision (15) << neg2LL_after + 2.075483954190555e+03  << endl;
      //assert(abs(neg2LL_before - 30795.5) <= 0.05);
      //assert(abs(neg2LL_after + 2.07478e+03)<= 1);
    }
  return 0;
}
