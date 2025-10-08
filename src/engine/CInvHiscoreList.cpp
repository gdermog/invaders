//****************************************************************************************************
//! \file CInvHiscore.cpp
//! Module contains class CInvHiscore, which implements ...
//****************************************************************************************************
//
//****************************************************************************************************
// 3. 10. 2025, V. Pospíšil, gdermog@seznam.cz
//****************************************************************************************************

#include <filesystem>

#include <CInvLogger.h>
#include <InvStringTools.h>
#include <engine/CInvHiscoreList.h>

namespace Inv
{

  static const std::string lModLogId( "HISCORE" );


  CInvHiscoreList::CInvHiscoreList( const std::string & inFileName ):
    mHiscoreList(),
    mFileName( inFileName )
  {
    Load();
  } // CInvHiscore::CInvHiscore

  //-------------------------------------------------------------------------------------------------

  CInvHiscoreList::~CInvHiscoreList()
  {
    Save();
  } // CInvHiscore::~CInvHiscore

  //-------------------------------------------------------------------------------------------------

  void CInvHiscoreList::Load()
  {
    bool autogenerate = false;

    if( !std::filesystem::exists( mFileName ) )
    {
      LOG << "Hiscore file '" << mFileName << "' does not exist.";
      autogenerate = true;
    } // if

    std::ifstream inFile;
    inFile.open( mFileName, std::ifstream::in );
    if( !inFile.is_open() )
    {
      LOG << "Cannot open hiscore file '" << mFileName << "'.";
      autogenerate = true;
    } // if

    if( false == autogenerate )
    {
      std::string inLine;
      size_t lastLineRead = 0;

      while( !inFile.eof() && lastLineRead <mMaxHiscoreCount )
      {
        std::getline( inFile, inLine );
        if( inLine.empty() )
          continue;

        ++lastLineRead;

        auto sepPos = inLine.find( ',' );
        if( sepPos == std::string::npos )
          continue;

        uint32_t score = 0;
        auto scoreStr = inLine.substr( 0, sepPos );
        Trim( scoreStr );

        auto nameStr = inLine.substr( sepPos + 1 );
        Trim( nameStr );
        nameStr = nameStr.substr( 0, mMaxHiscoreNameLen );

        try
        {
          auto strValType = IsNumeric( scoreStr.c_str() );

          if( NumberType_t::kIndexNumeric == strValType   ||
              NumberType_t::kIntegerNumeric == strValType ||
              NumberType_t::kHexaNumeric == strValType )
            score = std::stoul( scoreStr );
        } // try
        catch( std::exception & e )
        {
          LOG << "Error reading hiscore file";
          LOG << "  -> " << e.what() << std::endl;
          autogenerate = true;
          break;
        } // catch
        catch( ... )
        {
          LOG << "Error reading hiscore file";
          autogenerate = true;
          break;
        } // catch

        if( !nameStr.empty() && 0 < score )
          mHiscoreList.emplace_back( score, nameStr );

      } // while

      inFile.close();

    } // if

    if( true == autogenerate )
    {
      mHiscoreList.clear();
      mHiscoreList.emplace_back( 1000000, "gemini" );
      mHiscoreList.emplace_back(  900000, "fomosa" );
      mHiscoreList.emplace_back(  800000, "pizzax" );
      mHiscoreList.emplace_back(  700000, "grudge" );
      mHiscoreList.emplace_back(  600000, "letaxo" );
      mHiscoreList.emplace_back(  500000, "buffon" );
      mHiscoreList.emplace_back(  400000, "rubens" );
      mHiscoreList.emplace_back(  300000, "blurry" );
      mHiscoreList.emplace_back(  200000, "randal" );
      mHiscoreList.emplace_back(  100000, "wasabi" );
      mHiscoreList.emplace_back(   90000, "scooby" );
      mHiscoreList.emplace_back(   80000, "parley" );
      mHiscoreList.emplace_back(   70000, "greens" );
      mHiscoreList.emplace_back(   60000, "stupid" );
      mHiscoreList.emplace_back(   50000, "putrid" );
      mHiscoreList.emplace_back(   40000, "boogey" );
      mHiscoreList.emplace_back(   30000, "maroon" );
      mHiscoreList.emplace_back(   20000, "purple" );
      mHiscoreList.emplace_back(   10000, "unholy" );
      mHiscoreList.emplace_back(    9000, "plague" );
    } // if

    std::sort( mHiscoreList.begin(), mHiscoreList.end(), []( auto & left, auto & right )
    {
      return  right.first < left.first;
    } );

  } // CInvHiscore::Load

  //-------------------------------------------------------------------------------------------------

  void CInvHiscoreList::Save()
  {
    std::ofstream outFIle;
    outFIle.open( mFileName, std::ofstream::out );
    if( !outFIle.is_open() )
    {
      LOG << "Cannot recreate hiscore file '" << mFileName << "'.";
    } // if

    size_t lastLineWritten = 0;
    for( auto it: mHiscoreList )
    {
      if( lastLineWritten >= mMaxHiscoreCount )
        break;
      outFIle << it.first << ", " << it.second << std::endl;
      ++lastLineWritten;
    } // for

    outFIle.close();

  } // CInvHiscore::Save

  //-------------------------------------------------------------------------------------------------

  void CInvHiscoreList::AddNewHiscore( uint32_t inScore, const std::string & inName )
  {
    if( inScore == 0 || inName.empty() )
      return;

    auto nameStr = inName;
    Trim( nameStr );
    nameStr = nameStr.substr( 0, mMaxHiscoreNameLen );

    mHiscoreList.emplace_back( inScore, nameStr );

    std::sort( mHiscoreList.begin(), mHiscoreList.end(), []( auto & left, auto & right )
    {
      return right.first < left.first;
    } );

  } // CInvHiscore::AddNewHiscore

  //-------------------------------------------------------------------------------------------------


} // namespace Inv
