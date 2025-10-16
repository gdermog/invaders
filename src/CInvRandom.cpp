//****************************************************************************************************
//! \file CInvBackground.h
//! Module declares class CInvRandom that represents a Mersenne Twister PRNG
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include "CInvRandom.h"


namespace Inv
{

  //-----------------------------------------------------------------------------------------------

  CInvRandom & CInvRandom::GetInstance()
  {
    static CInvRandom gInvRandom;
    return gInvRandom;
  } /*InvRandom::GetInstance*/

  //--------------------------------------------------------------------------------------------------

  CInvRandom::CInvRandom( unsigned seed )
  {
    SetSeed( seed );
  }

  //--------------------------------------------------------------------------------------------------

  CInvRandom::~CInvRandom() = default;

  //----------------------------------------------------------------------------------------------------

  void CInvRandom::SetSeed( unsigned seed )
  {
    fCount624 = 624;

    if( seed > 0 )
    {
      fMt[0] = seed;
      fSeed = seed;
    }
    else
    {
      time_t rawtime;
      time( &rawtime );
      fMt[0] = (unsigned)rawtime;
      fSeed = (unsigned)rawtime;

    } /*else*/

    // Používá multiplikátory z Knuth : "Art of Computer Programming" Svazek 2, 3. edice, str. 106
    for( int loop = 1; loop < 624; loop++ )
    {
      fMt[loop] = ( 1812433253 * ( fMt[loop - 1] ^ ( fMt[loop - 1] >> 30 ) ) + loop );
    } /*for*/

  } /*SetSeed*/

  //--------------------------------------------------------------------------------------------------------------

  unsigned CInvRandom::RndmUInt32()
  {
    unsigned y;

    const int  kM = 397;
    const int  kN = 624;
    const unsigned kTemperingMaskB = 0x9d2c5680;
    const unsigned kTemperingMaskC = 0xefc60000;
    const unsigned kUpperMask = 0x80000000;
    const unsigned kLowerMask = 0x7fffffff;
    const unsigned kMatrixA = 0x9908b0df;

    if( fCount624 >= kN )
    {
      int i;

      for( i = 0; i < kN - kM; i++ )
      {
        y = ( fMt[i] & kUpperMask ) | ( fMt[i + 1] & kLowerMask );
        fMt[i] = fMt[i + kM] ^ ( y >> 1 ) ^ ( ( y & 0x1 ) ? kMatrixA : 0x0 );
      } /*for*/

      for( ; i < kN - 1; i++ )
      {
        y = ( fMt[i] & kUpperMask ) | ( fMt[i + 1] & kLowerMask );
        fMt[i] = fMt[i + kM - kN] ^ ( y >> 1 ) ^ ( ( y & 0x1 ) ? kMatrixA : 0x0 );
      } /*for*/

      y = ( fMt[kN - 1] & kUpperMask ) | ( fMt[0] & kLowerMask );
      fMt[kN - 1] = fMt[kM - 1] ^ ( y >> 1 ) ^ ( ( y & 0x1 ) ? kMatrixA : 0x0 );
      fCount624 = 0;
    } /*if*/

    y = fMt[fCount624++];
    y ^= ( y >> 11 );
    y ^= ( ( y << 7 ) & kTemperingMaskB );
    y ^= ( ( y << 15 ) & kTemperingMaskC );
    y ^= ( y >> 18 );

    return y;
  } // CInvRandom::RndmUInt32

  //--------------------------------------------------------------------------------------------------------------

  double CInvRandom::Rndm()
  {

    unsigned y = RndmUInt32();
    if( y )
    {
      double retVal = ( (double)y * 2.3283064365386963e-10 );
      return retVal;
    }
    return Rndm();

  } /*CInvRandom::Rndm*/

  //--------------------------------------------------------------------------------------------------------------


  double CInvRandom::RndmQuick()
  {
    const double kCONS = 4.6566128730774E-10;
    fSeed = ( 1103515245 * fSeed + 12345 ) & 0x7fffffffUL;

    if( fSeed )
    {
      double retVal = kCONS * fSeed;
      return retVal;
    } /*if*/

    return Rndm();
  } /*CInvRandom::RndmQuick*/

} // namespace _project_math
