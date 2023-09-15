#if !defined(INTENSITYMANAGER)
#define INTENSITYMANAGER

//******************************************************************************
// This file is part of AmpTools, a package for performing Amplitude Analysis
//
// Copyright Trustees of Indiana University 2010, all rights reserved
//
// This software written by Matthew Shepherd, Ryan Mitchell, and
//                  Hrayr Matevosyan at Indiana University, Bloomington
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice and author attribution, this list of conditions and the
//    following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice and author attribution, this list of conditions and the
//    following disclaimer in the documentation and/or other materials
//    provided with the distribution.
// 3. Neither the name of the University nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// Creation of derivative forms of this software for commercial
// utilization may be subject to restriction; written permission may be
// obtained from the Trustees of Indiana University.
//
// INDIANA UNIVERSITY AND THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES,
// EXPRESS OR IMPLIED.  By way of example, but not limitation, INDIANA
// UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES OF MERCANTABILITY OR
// FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF THIS SOFTWARE OR
// DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS,
// OR OTHER RIGHTS.  Neither Indiana University nor the authors shall be
// held liable for any liability with respect to any claim by the user or
// any other party arising from use of the program.
//******************************************************************************

#include "IUAmpTools/AmpVecs.h"
#include "IUAmpTools/AmpParameter.h"

#include <string>
#include <vector>
#include <map>
#include <complex>

class ConfigurationInfo;
class NormIntInterface;

using namespace std;

class IntensityManager {

public:
  
  enum Type { kAmplitude, kMoment };
  
  IntensityManager( const vector< string >& reaction,
                    const string& reactionName);

  virtual ~IntensityManager() {}
  
  /**
   * This function returns the type of IntensityManager.  
   *
   * The types currently implimented are kAmplitude and kMoment for 
   * AmplitudeManager and MomentManager.
   */
  virtual Type type() const = 0;
  
  
  /**
   * This function should return the maximum number of doubles required to store
   * all factors for any terms for an event.  It is used by the framework for
   * memory allocation.
   */
  
  virtual unsigned int maxFactorStoragePerEvent() const = 0;
  
  /**
   * This function should return the number of doubles required to store
   * all terms for an event.  It should be equal to or smaller
   * than that returned by the maxFactorStoragePerEvent.  The
   * reason being that each term may be composed of multiple
   * factors (for multiple permutations in the case of an amplitude fit).
   * Once the factors are assembled into a term then, as long
   * as the factors don't change, this term (modulo the production
   * factor) remains constant throughout the fit.
   */
  
  virtual unsigned int termStoragePerEvent() const = 0;
  
  /**
   * This function should return the number of doubles required to
   * store optional user data for all factors and permutations
   * for a single event.
   */
  
  virtual unsigned int userVarsPerEvent() const = 0;

  /*
   * These functions perform computations based on the current state
   * of the IntensityManager and should be defined by the the derived class.
   */
  
  /**
   * This function triggers the calculation of optional user data
   * that can be stored in the AmpVecs class to expedite subsequent
   * calculations of amplitudes and integrals.
   */
  
  virtual void calcUserVars( AmpVecs& ampVecs ) const = 0;
  
  /**
   * This function caculates the various intensity terms for each data event.
   * Since it can also be used to update the terms, it returns true
   * if any term was reclaculated, false if there were no changes.
   *
   * \param[in,out] ampVecs a reference to the AmpVecs storage structure, four vectors
   * will be read from this class and term caculations will be written to
   * this class
   *
   * \see calcIntensities
   * \see calcSumLogIntensity
   * \see calcIntegrals
   */
  virtual vector<bool> calcTerms( AmpVecs& ampVecs ) const = 0;
  
  /**
   * This function calculates the intensity (one number) for all events and
   * stores it in the AmpVecs structure.  It returns the maximum intensity
   * in the data set, which is useful for accept/reject MC generation
   * algorithms.
   *
   * \param[in,out] ampVecs a reference to the AmpVecs storage structure,
   * four vectors will be read from this class and intensities written to it.
   *
   * \see calcAmplitudes
   * \see calcSumLogIntensity
   * \see calcIntegrals
   */
  virtual double calcIntensities( AmpVecs& ampVecs ) const = 0;

  /**
   * This function calculates and returns the sum of the log of the intensities
   * stored in the AmpVecs structure. 
   *
   * \param[in,out] ampVecs a reference to the ampVecs structure from which the
   * intensities are to be read.  (This structure may be modified and updated
   * by underlying calls to calcIntensities and calcTerms.)
   *
   * \see calcAmplitudes
   * \see calcIntensities
   * \see calcIntegrals
   */
  virtual double calcSumLogIntensity( AmpVecs& ampVecs ) const = 0;
  
  /**
   * This routine calculates the average values of A_iA_j*, which is useful
   * for normalizing the PDFs.  The results are stored in the AmpVecs object.
   *
   * \param[in,out] ampVecs a reference to the ampVecs structure from which the
   * terms and factors are to be read.
   *
   * \param[in] iNGenEvents the number of genereated events (N in the above
   * computation)
   *
   * \see calcAmplitudes
   * \see calcIntensities
   */
  virtual void calcIntegrals( AmpVecs& ampVecs, int iNGenEvents ) const = 0;
 
  /**
   * This function calculates the intensity for one event using a Kinematics
   * object.  This is only useful for diagnostics on a small number of events.
   * The calcIntensities method should be used to calculate intensities
   * for many events.
   *
   * \param[in] kinematics a pointer to a Kinematics object
   */
  double calcIntensity( const Kinematics* kinematics ) const;
  
  //
  // The functions below all return information that describes the
  // state and configuration of the AmplitudeManager.
  //
  
  /**
   * This function returns a vector of strings.  Each string corresponds to
   * the name of a unique amplitude in the amplitude manager.  The location
   * of each amplitude in the vector represents the ampIndex, an integer
   * corresponding to each amplitude that is largely used internally.
   *
   * \see ampIndex
   */
  const vector< string >& getTermNames() const;
  
  /**
   * The function returns a reference to the parameter that is acting as the
   * scale factor for an amplitude.  Even if the user does not set a scale
   * factor, by default a fixed parameter is created and its value is
   * inititalized to 1.
   */
  const AmpParameter& getScale( const string& name ) const;
  
  
  /**
   * This function returns a boolean indicating if any term in the
   * IntensityManager contains a free parameter than might be changing
   * with each fit iteration.  It is used to trigger the recalculation of
   * normalization integrals at each fit iteration.
   *
   * \see setParPtr
   * \see setParValue
   */
  virtual bool hasTermWithFreeParam() const = 0;
  
  /**
   * This function will return true if every amplitude factor can be
   * calculated from user-defined data variables.  In some instances
   * this flag is used to optimize memory consumption as it means
   * that the raw four-vectors are not needed for amplitude
   * computations.
   */
  
  virtual bool needsUserVarsOnly() const = 0;
  
  /**
   * This returns the internal index of a term.  It is useful for users
   * who may want to index data in an array.
   *
   * \param[in] termName the name of the term to get the index of
   *
   * \see getAmpNames
   */
  int termIndex( const string& termName ) const;
  
  /**
   * This returns the name of the final state that was optionally passed
   * to the constructor of the AmplitudeManager.
   */
  string reactionName() const {return m_reactionName;}
  
  /**
   * This return returns the production factor (\f$ V_i\f$) for a
   * a single term.  In general this is a complex number, for moment
   * fits it will have no imaginary part.
   *
   * \param[in] ampName the string identifying the amplitude
   *
   * \see hasTerm
   * \see setExternalProductionFactor
   * \see setDefaultProductionFactor
   * \see resetProductionFactors
   */
  complex< double > productionFactor( const string& termName ) const;
  
  /**
   * \overload
   *
   * \param[in] ampIndex the index of the amplitude
   */
  complex< double > productionFactor( int termIndex ) const;
  
  /**
   * This fills an array with the real and imaginary parts of the ith
   * production factor in the 2*i and 2*i+1 elements.  It is less
   * user friendly, but useful for repeated high-speed access to
   * the data
   * 
   * \param[out] prodArray array of size 2*n that will be filled
   */
  void prodFactorArray( double* array ) const;
  
  /**
   * This class returns a boolean indicating whether or not a term
   * with the provided name exists in the IntensityManager.
   *
   * \param[in] termName the string identifying the term
   *
   * \see productionFactor
   * \see setExternalProductionFactor
   * \see setDefaultProductionFactor
   * \see resetProductionFactor
   */
  bool hasTerm( const string& termName ) const;
    
  //
  // The functions below modify the state of the AmplitudeManager
  //
  
  /**
   * This function sets up the IntensityManager based on information
   * provided by a ConfigurationInfo object.  It is intended to be the
   * most user-friendly way of configuring the amplitude manager for
   * use.
   *
   * \param[in] configInfo a pointer to a ConfigurationInfo object
   */
  virtual void setupFromConfigurationInfo( const ConfigurationInfo* configInfo ) = 0;

  /**
   * This function adds a new term to the intensity calculation with the
   * name termName.  The unique index of this term is provided as a return
   * value, which may be helpful in optimization.
   *
   * \param[in] termName the name of the new term.  If the term already 
   * exists then no new term will be created and the index of the
   * old term will be returned (and a warning message will be printed).
   *
   * \param[in] scale (optional) the name of a parameter to use for the
   * the scale factor for the term.
   *
   * \see termIndex
   */
  
  int addTerm( const string& termName, const string& scale = "1.0" );
  
  /**
   * This is used to set the default production amplitude (\f$ V_i \f$) for
   * a specific amplitude.  The production amplitude is the complex number
   * that multiplies the decay amplitude (a collection of amplitude factors)
   * in the construction of the intensity.  When fitting, this production
   * amplitude is typically a free parameter or is constrained to some other
   * free parameter in the fit.  To generate Monte Carlo, it is useful to
   * be able to set this value to any arbitrary complex number
   *
   * \param[in] ampName the name of the amplitude that is having its
   * production amplitude set
   *
   * \param[in] prodAmp the complex value of the production amplitude to set
   *
   * \see productionAmp
   * \see setExternalProductionAmplitude
   * \see resetProductionAmplitudes
   */
  void setDefaultProductionFactor( const string& termName,
                                   complex< double > prodAmp );
  
  /**
   * This is used to set the memory location to which the AmplitudeManager
   * should look to find the production amplitude for a particular amplitude.
   * Note that the AmplitudeManager does not have control over this memory.
   * This relies on the user to create and preserve the memory that holds the
   * acutal values.  This is particularly useful when fitting where the
   * ParameterManager manages and updates the production amplitudes and
   * the AmplitudeManager can just look at this memory to recalculate the
   * intensity.
   *
   * \param[in] ampName the name of the amplitude to set the production
   * parameter for
   *
   * \param[in] prodAmpPtr a pointer to memory where the AmplitudeManager
   * can look for the production amplitude
   *
   * \see productionAmp
   * \see setDefaultProductionAmplitude
   * \see resetProductionAmplitudes
   */
  void setExternalProductionFactor( const string& ampName,
                                    const complex< double >* prodAmpPtr );
  
  /**
   * This tells an amplitude to use an external pointer to resolve the value
   * of some parameter inside of an amplitude.  The parameter is not a
   * production paramater as discussed above, but instead a parameter that
   * may be floating in the description of the decay, e.g., the mass or width
   * of some state.  This routine is here for convenience, one could also
   * get a list of all the amplitude factors and check each one for the parameter
   * and then use the functionality of the Amplitude class to set the parameter
   * pointer.
   *
   * \param[in] ampName the name of the amplitude that contains a factor that
   * contains the paramter of interest
   *
   * \param[in] parName the name of the parameter
   *
   * \param[in] ampParPtr a pointer to external memory that tells the amplitude
   * where to find the value of the parameter
   *
   * \see setParValue
   * \see Amplitude::setParPtr
   */
  virtual void setParPtr( const string& termName, const string& parName,
                          const double* ampParPtr );
  
  /**
   * This tells an amplitude to use a particular value for
   * some parameter inside of an amplitude.  The parameter is not a
   * production paramater as discussed above, but instead a parameter that
   * may be floating in the description of the decay, e.g., the mass or width
   * of some state.  This routine is here for convenience, one could also
   * get a list of all the amplitude factors and check each one for the parameter
   * and then use the functionality of the Amplitude class to set the parameter
   * value.
   *
   * \param[in] ampName the name of the amplitude that contains a factor that
   * contains the paramter of interest
   *
   * \param[in] parName the name of the parameter
   *
   * \param[in] ampParValue the value to which to set the parameter
   *
   * \see setParValue
   * \see Amplitude::setParValue
   */
  virtual void setParValue( const string& termName, const string& parName,
                            double ampParValue );
  
  /**
   * This function will be called whenever a parameter is updateded.  
   *
   * \see Amplitude::updatePar
   */
  virtual void updatePar( const string& parName ) const {}
  
  /**
   * This resets all of the production amplitudes to their default values.
   * If production amplitudes were referenced from external pointers, this
   * external referencing is broken by this call and the amplitudes are instead
   * derived by internally stored default values.
   */
  void resetProductionFactors();
  
  /**
   * This function can be used to set a flag to optimize subsequent
   * calls to calcTerms in the case that amplitudes have free parameters.
   * It uses functionality in the updatePar function to only force a
   * recalculation of the amplitude for a particular AmpVecs class if
   * one of the AmpParameters for that amplitude has changed since
   * the last calculation.  This should signficiantly enhance the speed
   * for fits with parameters floating in amplitudes, since there will
   * only be an expensive recomputation when MINUIT changes the parameter.
   *
   * \param[in] flag set to true to enable the optimization
   */
  void setOptimizeParIteration( bool flag ) { m_optimizeParIteration = flag; }
  
  /**
   * This function will allow the AmplitudeManager to clear four vectors
   * from memory if all amplitudes can be calculated from user variables.
   *
   * \param[in] flag set to true to enable the optimization
   */
  void setFlushFourVecsIfPossible( bool flag ) { m_flushFourVecsIfPossible = flag; }
  
  /**
   * If set to true, this flag will force the recalculation of the user
   * data every time that calcTerms is called.  This is needed in cases
   * where one reuses a common memory block multiple times with different
   * kinematics, like what happens in MC generation.
   *
   * \param[in] flag set to true to enable the optimization
   */
  void setForceUserVarRecalculation( bool flag ) {
    m_forceUserVarRecalculation = flag;
    m_flushFourVecsIfPossible = false;
  }
  
protected:
  
  // some internal members to optimize term recalculation
  bool m_needsUserVarsOnly;
  bool m_optimizeParIteration;
  bool m_flushFourVecsIfPossible;
  bool m_forceUserVarRecalculation;
  
private:
  
  string m_reactionName;
  
  // term name -> production term
  map< string, const complex< double >* > m_prodFactor;
  
  // term index -> production amplitude
  vector< const complex< double >* > m_prodFactorVec;
  
  // a vector of amplitude names -- keep also a set of term indices
  // these can be useful speeding up intesity calculations by removing
  // slower map element look-ups
  vector< string > m_termNames;
  map< string, int > m_termIndex;
  
  // a map to hold a set of default production factors
  map< string, complex< double > > m_defaultProdFactor;
  
  vector< AmpParameter > m_termScaleVec;
  
  static const char* kModule;
};


#endif
