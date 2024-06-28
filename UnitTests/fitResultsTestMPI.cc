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
#include "IUAmpTools/FitResults.h"
#include "DalitzDataIO/DalitzDataReader.h"
#include "DalitzAmp/BreitWigner.h"
#include "DalitzAmp/Constraint.h"
#include "IUAmpTools/report.h"


static const char* kModule = "fitResultsTestMPI";

using namespace std;


class unitTest {
    public:
    bool passed = true;
    vector<string> failedTests;
    vector<string> passedTests;
    void add(bool expr, string name) {
        passed = passed && expr;
        if (expr) {
            passedTests.push_back(name);
        } else {
            failedTests.push_back(name);
        }
    }
    bool summary() {
        assert(passed || failedTests.size() > 0);
        if (passed) {
            cout << "All unit tests passed." << endl;
        } else {
            cout << "The following unit tests failed:" << endl;
            for (const string& failedTest : failedTests) {
                cout << "* " << failedTest << endl;
            }
            if (passedTests.size() > 0) {
            cout << "The following unit tests were successful:" << endl;
            for (const string& passedTest : passedTests) {
                cout << "* " << passedTest << endl;
            }
            }
        }
        return passed;
    }
};

bool testFitResults(const FitResults* fitResults, string AmpToolsVersion) {
    string fitResultsFile = "models/fitResults";
    if (AmpToolsVersion == "mpi") {
        fitResultsFile += "MPI.txt";
    } else if (AmpToolsVersion == "gpu") {
        fitResultsFile += "GPU.txt";
    } else if (AmpToolsVersion == "mpigpu") {
        fitResultsFile += "MPIGPU.txt";
    } else {
        fitResultsFile += ".txt";
    }
    unitTest unit_test;
    ifstream fin;
    fin.open(fitResultsFile);
    double intensity_first;
    double intensity_second;
    double pd_first;
    double pd_second;
    double ppBase_real;
    double ppBase_imag;
    double ppConstrained_real;
    double ppConstrained_imag;
    double ppSymm_real;
    double ppSymm_imag;
    double bestMinimum;
    int num_parameters;
    fin >> intensity_first;
    fin >> intensity_second;
    fin >> pd_first;
    fin >> pd_second;
    fin >> ppBase_real;
    fin >> ppBase_imag;
    fin >> ppConstrained_real;
    fin >> ppConstrained_imag;
    fin >> ppSymm_real;
    fin >> ppSymm_imag;
    fin >> bestMinimum;
    fin >> num_parameters;
    cout << abs(intensity_first-fitResults->intensity().first) << endl;
    cout << abs(intensity_second-fitResults->intensity().second) << endl;
    cout << abs(pd_first-fitResults->phaseDiff("base::s1::R12","base::s1::R13").first) << endl;
    cout << abs(pd_second-fitResults->phaseDiff("base::s1::R12","base::s1::R13").second) << endl;
    cout << abs(ppBase_real-fitResults->productionParameter("base::s1::R12").real()) << endl;
    cout << abs(ppBase_imag-fitResults->productionParameter("base::s1::R12").imag()) << endl;
    cout << abs(ppConstrained_real-fitResults->productionParameter("constrained::s2::RC12").real()) << endl;
    cout << abs(ppConstrained_imag-fitResults->productionParameter("constrained::s2::RC12").imag()) << endl;
    cout << abs(ppSymm_real-fitResults->productionParameter("symmetrized_explicit::s4::RSE12").real()) << endl;
    cout << abs(ppSymm_imag-fitResults->productionParameter("symmetrized_explicit::s4::RSE12").imag()) << endl;
    cout << abs(bestMinimum-fitResults->bestMinimum()) << endl;
    int sz = fitResults->parNameList().size();
    cout << abs(num_parameters-sz);
    vector<double> parVals = fitResults->parValueList();
    //for (const double i : parVals) {
        
    //}

    cout << "_________________________________________" << endl;
    cout << "_________________________________________" << endl;
    cout << "_________________________________________" << endl;
    cout << fitResults->intensity().first << endl;
    cout << fitResults->intensity().second << endl;
    cout << fitResults->phaseDiff("base::s1::R12","base::s1::R13").first << endl;
    cout << fitResults->phaseDiff("base::s1::R12","base::s1::R13").second << endl;
    cout << fitResults->productionParameter("base::s1::R12").real() << endl;
    cout << fitResults->productionParameter("base::s1::R12").imag() << endl;
    cout << fitResults->productionParameter("constrained::s2::RC12").real() << endl;
    cout << fitResults->productionParameter("constrained::s2::RC12").imag() << endl;
    cout << fitResults->productionParameter("symmetrized_explicit::s4::RSE12").real() << endl;
    cout << fitResults->productionParameter("symmetrized_explicit::s4::RSE12").imag() << endl;
    cout << fitResults->bestMinimum() << endl;
    cout << sz << endl;
    for (const double i : parVals) {
        cout << i << endl;
    }
    return true;
}

int main( int argc, char* argv[] ) {
    MPI_Init( &argc, &argv );
    int rank;
    int size;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if (argc <= 1){
    report( INFO, kModule ) << "Usage:" << endl << endl;
    report( INFO, kModule ) << "\tfitResultsTest <base/mpi/gpu/mpigpu>" << endl << endl;
    return 0;
    }
    string AmpToolsVersion(argv[1]);
    string cfgname = "parserTest.cfg";
    ConfigFileParser parser(cfgname);
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
    AmpToolsInterfaceMPI::registerAmplitude(BreitWigner());
    AmpToolsInterfaceMPI::registerNeg2LnLikContrib(Constraint());
    AmpToolsInterfaceMPI::registerDataReader(DataReaderMPI<DalitzDataReader>());
    AmpToolsInterfaceMPI ATI(cfgInfo);
    if (rank==0) {
    cout << "________________________________________" << endl;
    cout << "Testing FitResults from AmpToolsInterface:" << endl;
    cout << "________________________________________" << endl;
    

    MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
    fitManager->setStrategy(1);

    fitManager->migradMinimization();
    ATI.finalizeFit();
    const FitResults* fitResults = ATI.fitResults();
    testFitResults(fitResults, AmpToolsVersion);
    }

    ATI.exitMPI();
    MPI_Finalize();

    return 0;
}