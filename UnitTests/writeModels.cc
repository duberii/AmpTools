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
    if (argc <= 1){
        cout << "Usage:" << endl << endl;
        cout << "\tparserTest <config file name>" << endl << endl;
        return 0;
    }
    ofstream fout;
    string cfgname(argv[1]);
    ConfigFileParser parser(cfgname);
    ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();


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
    
    return 0;
}