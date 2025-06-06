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
    void add(double valModel, double val, double tolerance, string name) {
        bool expr = abs(valModel-val)<= tolerance;
        passed = passed && expr;
        if (expr) {
            passedTests.push_back(name + "(diff="+to_string(abs(valModel-val)) +")");
        } else {
            failedTests.push_back(name + "(diff="+to_string(abs(valModel-val)) +")");
        }
    }
    bool summary() {
        assert(passed || failedTests.size() > 0);
        if (passed) {
            cout << "All unit tests passed." << endl;
            for (const string& passedTest : passedTests) {
                cout << "* " << passedTest << endl;
            }
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

int main( int argc, char* argv[] ) {
    string ATIFile = "models/AmpToolsInterface.txt";
    bool result;
    MPI_Init( &argc, &argv );
    int rank;
    int size;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    string cfgname = "parserTest.cfg";
    ConfigFileParser parser(cfgname);
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
    AmpToolsInterfaceMPI::registerAmplitude(BreitWigner());
    AmpToolsInterfaceMPI::registerNeg2LnLikContrib(Constraint());
    AmpToolsInterfaceMPI::registerDataReader(DataReaderMPI<DalitzDataReader>());
    AmpToolsInterfaceMPI ATI(cfgInfo);
    AmpToolsInterfaceMPI::setRandomSeed(12345);
    if (rank == 0){
    cout << "________________________________________" << endl;
    cout << "Testing AmpToolsInterface from ConfigurationInfo:" << endl;
    cout << "________________________________________" << endl;
    
    unitTest unit_test;
    ifstream fin;
    fin.open(ATIFile);

    double neg2LL_before;
    fin >> neg2LL_before;
    unit_test.add(neg2LL_before, ATI.likelihood(), 1e-1, "Likelihood before fit matches model");
    MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
    fitManager->setStrategy(1);

    fitManager->migradMinimization();

    double neg2LL_after;
    fin >> neg2LL_after;
    unit_test.add(neg2LL_after, ATI.likelihood(), 1e-1, "Likelihood after fit matches model");
    double neg2LL_base;
    fin >> neg2LL_base;
    unit_test.add(neg2LL_base, ATI.likelihood("base"), 1e-3, "Likelihood of base reaction after fit matches model");
    double neg2LL_constrained;
    fin >> neg2LL_constrained;
    unit_test.add(neg2LL_constrained, ATI.likelihood("constrained"),1e-3, "Likelihood of constrained reaction after fit matches model");
    double neg2LL_symmetrized_implicit;
    fin >> neg2LL_symmetrized_implicit;
    unit_test.add(neg2LL_symmetrized_implicit,ATI.likelihood("symmetrized_implicit"),1e-4, "Likelihood of symmetrized (implicit) reaction after fit matches model");
    double neg2LL_symmetrized_explicit;
    fin >> neg2LL_symmetrized_explicit;
    unit_test.add(neg2LL_symmetrized_explicit, ATI.likelihood("symmetrized_explicit"), 1e-4, "Likelihood of symmetrized (explicit) reaction after fit matches model");
    result = unit_test.summary();
    if (!result) {
        throw runtime_error("Unit Tests Failed. See previous logs for more information.");
    }
    }
    ATI.exitMPI();
    MPI_Finalize();
    return 0;
}