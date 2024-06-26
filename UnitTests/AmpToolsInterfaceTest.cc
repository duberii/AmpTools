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

int main() {
    string cfgname = "parserTest.cfg";
    ConfigFileParser parser(cfgname);
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
    AmpToolsInterface::registerAmplitude(BreitWigner());
    AmpToolsInterface::registerNeg2LnLikContrib(Constraint());
    AmpToolsInterface::registerDataReader(DalitzDataReader());
    AmpToolsInterface ATI(cfgInfo);
    cout << "________________________________________" << endl;
    cout << "Testing AmpToolsInterface from ConfigurationInfo:" << endl;
    cout << "________________________________________" << endl;
    
    unitTest unit_test;
    ifstream fin;
    fin.open("models/AmpToolsInterface.txt");

    double neg2LL_before;
    fin >> neg2LL_before;
    cout << ATI.likelihood() << endl;
    unit_test.add(abs(ATI.likelihood()-neg2LL_before)<= 1e-11, "Likelihood before fit matches model");

    MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
    fitManager->setStrategy(1);

    fitManager->migradMinimization();

    double neg2LL_after;
    fin >> neg2LL_after;
    cout << ATI.likelihood() << endl;
    unit_test.add(abs(ATI.likelihood()-neg2LL_after)<= 1e-11, "Likelihood after fit matches model");
    double neg2LL_base;
    fin >> neg2LL_base;
    cout << ATI.likelihood("base") << endl;
    unit_test.add(abs(ATI.likelihood("base")-neg2LL_base)<= 1e-11, "Likelihood of base reaction after fit matches model");
    double neg2LL_constrained;
    fin >> neg2LL_constrained;
    cout << ATI.likelihood("constrained") << endl;
    unit_test.add(abs(ATI.likelihood("constrained")-neg2LL_constrained)<= 1e-11, "Likelihood of constrained reaction after fit matches model");
    double neg2LL_symmetrized_implicit;
    fin >> neg2LL_symmetrized_implicit;
    cout << ATI.likelihood("symmetrized_implicit") << endl;
    unit_test.add(abs(ATI.likelihood("symmetrized_implicit")-neg2LL_symmetrized_implicit)<= 1e-11, "Likelihood of symmetrized (implicit) reaction after fit matches model");
    double neg2LL_symmetrized_explicit;
    fin >> neg2LL_symmetrized_explicit;
    cout << ATI.likelihood("symmetrized_explicit") << endl;
    unit_test.add(abs(ATI.likelihood("symmetrized_explicit")-neg2LL_symmetrized_explicit)<= 1e-11, "Likelihood of symmetrized (explicit) reaction after fit matches model");
    bool result = unit_test.summary();
    if (!result) {
        throw runtime_error("Unit Tests Failed. See previous logs for more information.");
    }
    return 0;
}