//Copyright SimBlocks LLC 2016-2026
/**
 * @file Logger.cpp
 * @brief Implements logging functionality for message output.
 *
 * This file provides the implementation of the CLogger class, which supports logging
 * messages at various levels (debug, error, information, warning) to console and file.
 * Integrates with Poco::Logger for advanced logging features and log level control.
 *
 * @see sbio::utils::CLogger
 */
#include "Logger.h"
#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Logger.h"
#include "Poco/Message.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/SplitterChannel.h"
#include <cstdio>
#include <sstream>

using namespace std;
using namespace Poco;
using namespace sbio::utils;

// Constructs a logger with the specified name and options.
CLogger::CLogger(const std::string& sName, bool bLogToConsole, bool bLogToFile, ELogLevel eLogLevel) : m_pLogger(nullptr), m_sName(sName), m_bLogToFile(bLogToFile)
{
  if (Poco::Logger::has(sName))
  {
    m_pLogger = &Poco::Logger::get(sName);
    return;
  }

  string sFile = sName + ".log";

  if (m_bLogToFile)
  {
    std::remove(sFile.c_str());
  }

  AutoPtr<SplitterChannel> pSplitterChannel = new SplitterChannel();

  if (bLogToConsole)
  {
    AutoPtr<ConsoleChannel> pConsoleChannel(new ConsoleChannel);
    pSplitterChannel->addChannel(pConsoleChannel);
  }

  if (m_bLogToFile)
  {
    AutoPtr<SimpleFileChannel> pSimpleFileChannel(new SimpleFileChannel);
    pSimpleFileChannel->setProperty("path", sFile);
    pSplitterChannel->addChannel(pSimpleFileChannel);
  }

  int nLogLevel = Message::PRIO_ERROR;

  switch (eLogLevel)
  {
  case ELogLevel::LOG_ERROR:
    nLogLevel = Message::PRIO_ERROR;
    break;
  case ELogLevel::LOG_INFORMATION:
    nLogLevel = Message::PRIO_INFORMATION;
    break;
  case ELogLevel::LOG_WARNING:
    nLogLevel = Message::PRIO_WARNING;
    break;
  case ELogLevel::LOG_DEBUG:
    nLogLevel = Message::PRIO_DEBUG;
    break;
  default:
    break;
  }

  m_pLogger = &Poco::Logger::create(sName, pSplitterChannel, nLogLevel);
}

// Destructor. Cleans up logger resources.
CLogger::~CLogger()
{
  m_pLogger = nullptr;
}

void CLogger::Log(ELogLevel eLogLevel, const std::string& sText) const
{
  if (eLogLevel == ELogLevel::LOG_ERROR)
  {
    LogError(sText);
  }
  else if (eLogLevel == ELogLevel::LOG_WARNING)
  {
    LogWarning(sText);
  }
  else if (eLogLevel == ELogLevel::LOG_INFORMATION)
  {
    LogInformation(sText);
  }
  else if (eLogLevel == ELogLevel::LOG_DEBUG)
  {
    LogDebug(sText);
  }
}

// Logs a debug message.
void CLogger::LogDebug(const std::string& sText) const
{
  try
  {
    m_pLogger->debug(sText);
  }
  catch (const Poco::Exception&)
  {
  }
}

// Logs an error message from a stringstream.
void CLogger::LogError(const std::stringstream& stream) const
{
  LogError(stream.str());
}

// Logs an error message from a string.
void CLogger::LogError(const std::string& sText) const
{
  try
  {
    m_pLogger->error("Error: " + sText);
  }
  catch (const Poco::Exception&)
  {
  }
}

// Logs an informational message from a stringstream.
void CLogger::LogInformation(const std::stringstream& stream) const
{
  try
  {
    LogInformation(stream.str());
  }
  catch (const Poco::Exception&)
  {
  }
}

// Logs an informational message from a string.
void CLogger::LogInformation(const std::string& sText) const
{
  try
  {
    m_pLogger->information(sText);
  }
  catch (const Poco::Exception&)
  {
  }
}

// Logs a warning message.
void CLogger::LogWarning(const std::string& sText) const
{
  try
  {
    m_pLogger->warning("Warning: " + sText);
  }
  catch (const Poco::Exception&)
  {
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
