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

/*! \brief Třída představuje generátor náhodných čísel metodou "Mersenne Twister". Popis metody
            viz  http://en.wikipedia.org/wiki/Mersenne_twister . */
  class CInvRandom
  {
  public:

    static CInvRandom & GetInstance();
    //!< Tato metoda vrátí globální instanci třídy CConfig. Třída není přísný singleton, lze ji
    //!  instanciovat znovu například pro potřeby editace popisů konfiguračních položek (admin
    //!  tool).



    ~CInvRandom();

    void SetSeed( unsigned seed = 0 );
    /*!< \brief Inicializuje generátor pseudonáhodných čísel zadaným semínkem. Je-li zadána nula, za
                semínko je vzat aktuální čas. Defaultově je generátor inicializován jedničkou.
         \param [in]  seed  Inicializační semínko */

    unsigned RndmUInt32();

    double Rndm();
    //!< \brief Vygeneruje pseudonáhodné číslo z rozsahu (0,1)

    double Rndm( double min, double max ) { return min + ( max - min ) * Rndm(); }
    //!< \brief Vygeneruje pseudonáhodné číslo ze zadaného rozsahu

    double RndmQuick();
    //!< \brief Vygeneruje rychlé pseudonáhodné číslo z rozsahu (0,1) rychlejším algoritmem než Rndm()

    double RndmQuick( double min, double max ) { return min + ( max - min ) * RndmQuick(); }
    //!< \brief Vygeneruje rychlé pseudonáhodné číslo ze zadaného rozsahu rychlejším algoritmem než Rndm()

  private:

    CInvRandom( unsigned seed = 1 );

    unsigned  fMt[624];
    int       fCount624;
    unsigned  fSeed;

  }; // CInvRandom


  inline float InvRnd() { return (float)CInvRandom::GetInstance().Rndm(); }

  inline unsigned InvRndUInt32() { return CInvRandom::GetInstance().RndmUInt32(); }

} // namespace Inv

#endif
