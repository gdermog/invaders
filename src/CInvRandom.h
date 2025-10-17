//****************************************************************************************************
//! \file CInvBackground.h
//! Module declares class CInvRandom that represents a Mersenne Twister PRNG
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************


#ifndef H_CInvRandom
#define H_CInvRandom

#include <InvGlobals.h>

namespace Inv
{

  /*! \brief This singleton class represents a random number generator using the "Mersenne Twister" method.
      For a description of the method see http://en.wikipedia.org/wiki/Mersenne_twister . */
  class CInvRandom
  {
  public:

    static CInvRandom & GetInstance();

    ~CInvRandom();

    void SetSeed( unsigned seed = 0 );
    /*!< \brief Initializes the pseudorandom number generator with the specified seed. If zero is specified,
          the current time is taken as the seed. By default, the generator is initialized with one.

         \param [in] seed Initialization seed */

    unsigned RndmUInt32();
    //!< \brief Generates a pseudorandom unsigned 32-bit integer

    double Rndm();
    //!< \brief Generates a pseudorandom number from the range (0,1)

    double Rndm( double min, double max ) { return min + ( max - min ) * Rndm(); }
    //!< \brief Generates a pseudorandom number from the specified range

    double RndmQuick();
    //!< \brief Generates a fast pseudorandom number from the range (0,1) with a faster algorithm than Rndm()

    double RndmQuick( double min, double max ) { return min + ( max - min ) * RndmQuick(); }
    //!< \brief Generates a fast pseudorandom number from the specified range using a faster algorithm than Rndm()

  private:

    CInvRandom( unsigned seed = 1 );

    unsigned  fMt[624];
    //<! \brief State vector

    int       fCount624;
    //<! \brief Current position in state vector

    unsigned  fSeed;
    //<! \brief Current seed

  }; // CInvRandom


  inline float InvRnd() { return (float)CInvRandom::GetInstance().Rndm(); }
  //<! \brief Convenient function, calls PRNG singleton and generates a pseudorandom float from the range (0,1)

  inline unsigned InvRndUInt32() { return CInvRandom::GetInstance().RndmUInt32(); }
  //<! \brief Convenient function, calls PRNG singleton and  generates a pseudorandom unsigned 32-bit integer

} // namespace Inv

#endif
