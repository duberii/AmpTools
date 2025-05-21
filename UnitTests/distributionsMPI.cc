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
static const char* kModule = "distributionsMPI";
using namespace std;

int main( int argc, char* argv[] )
{
    ofstream fout;
    AmpToolsInterfaceMPI::registerAmplitude(BreitWigner());
    AmpToolsInterfaceMPI::registerNeg2LnLikContrib(Constraint());
    AmpToolsInterfaceMPI::registerDataReader(DataReaderMPI<DalitzDataReader>());
    for (int i = 0; i < 1000; i++) {
        bool result;
        MPI_Init( &argc, &argv );
        int rank;
        int size;
        MPI_Comm_rank( MPI_COMM_WORLD, &rank );
        MPI_Comm_size( MPI_COMM_WORLD, &size );
        string distFile = "models/distFile.csv";
        string cfgname("parserTest.cfg");
        ConfigFileParser parser(cfgname);
        ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
        AmpToolsInterfaceMPI ATI(cfgInfo);

        // AmpToolsInterface
        
        fout.open(distFile);
        double neg2LL_before = ATI.likelihood();
        fout << neg2LL_before << ",";

        MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
        fitManager->setStrategy(1);

        fitManager->migradMinimization();

        double neg2LL_after = ATI.likelihood();
        fout << neg2LL_after << ",";
        fout << ATI.likelihood("base") << ",";
        fout << ATI.likelihood("constrained") << ",";
        fout << ATI.likelihood("symmetrized_implicit") << ",";
        fout << ATI.likelihood("symmetrized_explicit") << ",";
        ATI.finalizeFit();

        // fitResults

        const FitResults* fitResults = ATI.fitResults();
        pair<double, double> intensity = fitResults->intensity();
        fout << intensity.first << ",";
        fout << intensity.second << ",";
        pair<double, double> pd = fitResults->phaseDiff("base::s1::R12", "base::s1::R13");
        fout << pd.first << ",";
        fout << pd.second << ",";
        complex<double> ppBase = fitResults->productionParameter("base::s1::R12");
        fout << ppBase.real() << ",";
        fout << ppBase.imag() << ",";
        complex<double> ppConstrained = fitResults->productionParameter("constrained::s2::RC12");
        fout << ppConstrained.real() << ",";
        fout << ppConstrained.imag() << ",";
        complex<double> ppSymm = fitResults->productionParameter("symmetrized_explicit::s4::RSE12");
        fout << ppSymm.real() << ",";
        fout << ppSymm.imag() << ",";
        double bestMinimum = fitResults->bestMinimum();
        fout << bestMinimum << ",";
        vector<string> parNames = fitResults->parNameList();
        fout << parNames.size() << ",";
        vector<double> parVals = fitResults->parValueList();
        for (const double i : parVals) {
            fout << i << ",";
        }
        fout << "\n";
    }
    fout.close();
    ATI.exitMPI();
    MPI_Finalize();
    return 0;
}
