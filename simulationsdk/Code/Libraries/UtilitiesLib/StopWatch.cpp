//Copyright SimBlocks LLC 2016-2026
/**
 * @file StopWatch.cpp
 * @brief Implements high-resolution timing using Poco::Stopwatch.
 *
 * This file provides the implementation of the CStopWatch class, which wraps Poco::Stopwatch
 * to measure elapsed time in microseconds, milliseconds, and seconds. Supports starting,
 * stopping, resetting, and checking the running state of the timer.
 *
 * @see sbio::utils::CStopWatch
 */
#include "StopWatch.h"
#include "Poco/Clock.h"

using namespace std;
using namespace Poco;
using namespace sbio::utils;

// Constructs a stopwatch instance.
CStopWatch::CStopWatch()
{
  m_pStopwatch = make_unique<Stopwatch>();
}

// Destructor. Cleans up stopwatch resources.
CStopWatch::~CStopWatch()
{
}

// Gets the elapsed time in microseconds.
uint64_t CStopWatch::GetElapsedMicroseconds() const
{
  return static_cast<uint64_t>(m_pStopwatch->elapsed());
}

// Gets the elapsed time in milliseconds.
double CStopWatch::GetElapsedMilliSeconds() const
{
  //elapsed returns microseconds
  return static_cast<double>(m_pStopwatch->elapsed()) / 1000.0;
}

// Gets the elapsed time in seconds.
double CStopWatch::GetElapsedSeconds() const
{
  return GetElapsedMilliSeconds() / 1000.0;
}

// Checks if the stopwatch is currently running.
bool CStopWatch::IsRunning() const
{
  return m_bRunning;
}

// Resets the stopwatch to zero and stops it.
void CStopWatch::Reset()
{
  m_bRunning = false;
  m_pStopwatch->reset();
}

// Starts the stopwatch.
void CStopWatch::Start()
{
  m_bRunning = true;
  m_pStopwatch->start();
}

// Stops the stopwatch.
void CStopWatch::Stop()
{
  m_bRunning = false;
  m_pStopwatch->stop();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
