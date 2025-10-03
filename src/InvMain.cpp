//****************************************************************************************************
//! \file InvMain.cpp                                                                      
//! Module contains main entrypoint for the application.                                                                            
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <iomanip>
#include <filesystem>

#include <InvGlobals.h>
#include <CInvLogger.h>
#include <CInvConfig.h>
#include <CInvSettings.h>
#include <CInvGame.h>

static const std::string lModLogId( "MAIN" );

//******* Command line help **************************************************************************

std::ostream & HlpLine()
{
  static const std::string hMargin( Inv::gHelpMarginWidth, ' ' );
  return std::cout << hMargin << std::setw( Inv::gHelpItemWidth ) << std::left;
} // HlpLine

void PrintCommandlineHelp()
{

  std::cout << std::endl << std::endl;
  std::cout << "Command line expected values: " << std::endl << std::endl;

  HlpLine() << "--help" << "Print this help" << std::endl;
  HlpLine() << "--setup <File name>" << "Path to INI file containing setup, default invaders.ini" << std::endl;

  std::cout << std::endl << std::endl;
  std::cout << "INI file expected values: " << std::endl << std::endl;

  std::cout << std::endl << std::endl;

} // PrintCommandlineHelp

//******* Main function ******************************************************************************

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR cmd, INT )
{
  //------ Open log file --------------------------------------------------------------------------------

  auto & log = Inv::CInvLoggger::GetInstance();
  if( !log.IsOpen() )
  {
    std::cerr << "Cannot open log file 'invaders.log', quitting." << std::endl;
    return -1;
  } // if

  //------ Import command line arguments -------------------------------------------------------------

  Inv::CInvConfig cfg;
  cfg.ParseCommandLine( __argc, __argv );

  if( cfg.GetValueBool( {}, "help" ) || cfg.GetValueBool( {}, "h" ) )
  {
    PrintCommandlineHelp();
    return 0;
  } // if

  //------ Import settings from configuration file ---------------------------------------------------

  auto inFileName = cfg.GetValueStr( {}, "setup", "invaders.ini" );
  if( inFileName.empty() )
  {
    LOG << "No setup file specified, use --setup <file name>.";
    PrintCommandlineHelp();
    return -1;
  } // if

  if( !std::filesystem::exists( inFileName ) )
  {
    LOG << "Setup file '" << inFileName << "' does not exist.";
    return -1;
  } // if

  std::ifstream inFile;
  inFile.open( inFileName, std::ifstream::in );
  if( !inFile.is_open() )
  {
    LOG << "Cannot open setup file '" << inFileName << "'.";
    return -1;
  } // if

  size_t lastLineRead = 0;
  if( !cfg.ParseINIFile( inFile, lastLineRead ) )
  {
    LOG << "Error reading setup file '" << inFileName << "', problem on line ." << lastLineRead;
    return -1;
  } // if

  inFile.close();

  Inv::CInvSettings gameSettings;
  gameSettings.ImportSettings( cfg );
  gameSettings.Preprint();

  //------ Start the game -----------------------------------------------------------------------------
  
  Inv::CInvGame game( gameSettings );

  if( !game.Initialize() )
  {
    LOG << "Game initialization failed, quitting.";
    return -1;
  } // if

  if( !game.Run() )
  {
    LOG << "Game run failed, quitting.";
    return -1;
  } // if

  if( !game.Cleanup() )
  {
    LOG << "Game cleanup failed.";
    return -1;
  } // if

  return 0;

} // WinMain

