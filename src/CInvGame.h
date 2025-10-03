//****************************************************************************************************
//! \file CInvGame.h                                                                          
//! Module contains class CInvGame, which implements singleton pattern for global logging                                                                      
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CInvGame
#define H_CInvGame

#include <InvGlobals.h>
#include <CInvSettings.h>

namespace Inv
{

  class CInvGame
  {
  public:

    CInvGame( const CInvSettings & settings );

    CInvGame( const CInvGame & ) = delete;
    CInvGame & operator=( const CInvGame & ) = delete;
    ~CInvGame();

    bool Initialize();
    //!< Initializes game, returns true if successful

    bool Run();
    //!< Runs the game, returns true if successful

    bool Cleanup();
    //!< Releases resources of the game, returns true if successful


  private:

    const CInvSettings & mSettings;

  };

} // namespace Inv

#endif
