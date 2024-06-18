#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <mpi.h>
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/ConfigurationInfo.h"
#include "IUAmpToolsMPI/AmpToolsInterfaceMPI.h"
#include "IUAmpToolsMPI/DataReaderMPI.h"
#include "DalitzDataIO/DalitzDataReader.h"
#include "DalitzAmp/BreitWigner.h"
#include "DalitzAmp/Constraint.h"

#include "IUAmpTools/report.h"
static const char* kModule = "fitAmplitudesMPI";

using std::complex;
using namespace std;

int main( int argc, char* argv[] ){

  MPI_Init( &argc, &argv );
  
  int rank;
  int size;
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );


    // ************************
    // usage
    // ************************


  if (argc <= 1){
    report( INFO, kModule ) << "Usage:" << endl << endl;
    report( INFO, kModule ) << "\tfitAmplitudesMPI <config file name>" << endl << endl;
    MPI_Finalize();
    return 0;
  }

  report( INFO, kModule ) << " *** Performing the Fit *** " << endl;


    // ************************
    // parse the command line parameters
    // ************************

  string cfgname(argv[1]);

  report( INFO, kModule ) << "Config file name:  " << cfgname << endl << endl;


    // ************************
    // parse the config file
    // ************************

  ConfigFileParser parser(cfgname);
  ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
  cfgInfo->display();


    // ************************
    // AmpToolsInterface
    // ************************

  AmpToolsInterfaceMPI::registerAmplitude(BreitWigner());
  AmpToolsInterfaceMPI::registerNeg2LnLikContrib(Constraint());
  AmpToolsInterfaceMPI::registerDataReader(DataReaderMPI<DalitzDataReader>());

  AmpToolsInterfaceMPI ATI(cfgInfo);

  if (rank == 0){
    
    double neg2LL_before = ATI.likelihood();
    MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
    fitManager->setStrategy(1);

    fitManager->migradMinimization();

    if( fitManager->status() != 0 && fitManager->eMatrixStatus() != 3 ){
      report( WARNING, kModule ) << "Fit failed." << endl;
    }

    double neg2LL_after = ATI.likelihood();

    ATI.finalizeFit();

    if (cfgname == "dalitz1.cfg") {
      cout << abs(neg2LL_before - 30795.5479890) << endl;
      cout << abs(neg2LL_after + 2.07478131718e+03) << endl;
      //assert(abs(neg2LL_before - 30795.5) <= 0.05);
      //assert(abs(neg2LL_after + 2.07478e+03)<= 0.005);
    } else if (cfgname == "dalitz2.cfg") {
      cout << abs(neg2LL_before - 66474.2214262) << endl;
      cout << abs(neg2LL_after + 5.54876246163e+03) << endl;
      //assert(neg2LL_before == 66474.2);
      //assert(neg2LL_after == -5.54876e+03);
    } else if (cfgname == "dalitz3.cfg") {
      cout << abs(neg2LL_before - 30795.5) << endl;
      cout << abs(neg2LL_after + 2.07548e+03) << endl;
      //assert(abs(neg2LL_before - 30795.5) <= 0.05);
      //assert(abs(neg2LL_after + 2.07478e+03)<= 1);
    }}


  ATI.exitMPI();
  MPI_Finalize();

  return 0;

}