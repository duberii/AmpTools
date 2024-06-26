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

int main( int argc, char* argv[] ) {
    string AmpToolsVersion(argv[1]);
    ofstream fout;
    string cfgname("parserTest.cfg");
    ConfigFileParser parser(cfgname);
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
    AmpToolsInterface::registerAmplitude(BreitWigner());
    AmpToolsInterface::registerNeg2LnLikContrib(Constraint());
    AmpToolsInterface::registerDataReader(DalitzDataReader());
    AmpToolsInterface ATI(cfgInfo);


    //ConfigFileParser
    vector<ConfigFileLine> cfgLines = parser.getConfigFileLines();
    fout.open("models/parsedConfig.txt");
    vector<string> cfgStrings;
    for (const ConfigFileLine cfgLine : cfgLines) {
        fout << cfgLine.line() << "\n";
    }
    fout.close();

    //ConfigurationInfo
    fout.open("models/configurationInfo.txt");
    fout << cfgInfo->fitName() << "\n";
    fout << cfgInfo->fitOutputFileName() << "\n";
    vector<string> kwds = cfgInfo->userKeywords();
    fout << kwds.size()<<"\n";
    vector<ReactionInfo*> rinfo = cfgInfo->reactionList();
    fout << rinfo.size() << "\n";
    vector<AmplitudeInfo*> ainfo= cfgInfo->amplitudeList();
    fout << ainfo.size() << "\n";
    vector<CoherentSumInfo*> csinfo= cfgInfo->coherentSumList();
    fout << csinfo.size() << "\n";
    vector<Neg2LnLikContribInfo*> llinfo= cfgInfo->neg2LnLikContribList();
    fout << llinfo.size() << "\n";
    vector<PDFInfo*> pdfinfo= cfgInfo->pdfList();
    fout << pdfinfo.size() << "\n";
    vector<TermInfo*> tinfo= cfgInfo->termList();
    fout << tinfo.size() << "\n";
    vector<ParameterInfo*> pinfo = cfgInfo->parameterList();
    fout << pinfo.size() << "\n"; 

    fout.close();

    //AmpToolsInterface
    string ATIFile = "models/AmpToolsInterface";
    if (AmpToolsVersion == "mpi") {
        ATIFile += "MPI.txt";
    } else if (AmpToolsVersion == "gpu") {
        ATIFile += "GPU.txt";
    } else if (AmpToolsVersion == "mpigpu") {
        ATIFile += "MPIGPU.txt";
    } else {
        ATIFile += ".txt";
    }
    fout.open(ATIFile);
    double neg2LL_before = ATI.likelihood();
    fout << setprecision(15) << neg2LL_before << "\n";

    MinuitMinimizationManager* fitManager = ATI.minuitMinimizationManager();
    fitManager->setStrategy(1);

    fitManager->migradMinimization();


    double neg2LL_after = ATI.likelihood();
    fout << setprecision(15) << neg2LL_after << "\n";
    fout << setprecision(15) << ATI.likelihood("base") << "\n";
    fout << setprecision(15) << ATI.likelihood("constrained") << "\n";
    fout << setprecision(15) << ATI.likelihood("symmetrized_implicit") << "\n";
    fout << setprecision(15) << ATI.likelihood("symmetrized_explicit") << "\n";
    ATI.finalizeFit();
    fout.close();
    
    
    return 0;
}