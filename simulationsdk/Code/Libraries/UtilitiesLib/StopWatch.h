//Copyright SimBlocks LLC 2016-2026
/**
 * @file StopWatch.h
 * @brief Declares a wrapper class for high-resolution timing using Poco::Stopwatch.
 *
 * Defines the CStopWatch class, which provides methods for measuring elapsed time in
 * microseconds, milliseconds, and seconds. Supports starting, stopping, resetting, and
 * checking the running state of the timer.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_STOPWATCH_H
#define SIMBLOCKS_UTILITIES_STOPWATCH_H

#include "Poco/Stopwatch.h"
#include <cstdint>
#include <memory>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Lightweight wrapper around `Poco::Stopwatch`.
     *
     * `CStopWatch` exposes elapsed time in multiple units and keeps a separate boolean flag that tracks
     * whether `Start()` has most recently been called without a matching `Stop()` or `Reset()`.
     *
     * @invariant `m_pStopwatch` is non-null after successful construction.
     * @invariant `m_bRunning` reflects the wrapper's logical running state.
     */
    class CStopWatch
    {
    public:
      /**
       * @brief Constructs a stopwatch instance.
       *
       * @failurecases Memory allocation for the underlying `Poco::Stopwatch` may throw.
       */
      CStopWatch();

      /**
       * @brief Destroys the stopwatch wrapper.
       */
      ~CStopWatch();

      /**
       * @brief Gets elapsed time in microseconds.
       * @return Current elapsed time reported by the underlying stopwatch.
       */
      uint64_t GetElapsedMicroseconds() const;

      /**
       * @brief Gets elapsed time in milliseconds.
       * @return Current elapsed time reported by the underlying stopwatch divided by `1000.0`.
       */
      double GetElapsedMilliSeconds() const;

      /**
       * @brief Gets elapsed time in seconds.
       * @return Current elapsed time in seconds.
       */
      double GetElapsedSeconds() const;

      /**
       * @brief Reports whether the stopwatch is logically running.
       * @return `true` after `Start()` and before the next `Stop()` or `Reset()`; otherwise `false`.
       */
      bool IsRunning() const;

      /**
       * @brief Resets elapsed time and marks the stopwatch as not running.
       *
       * @sideeffects Clears the accumulated elapsed time in the underlying stopwatch.
       */
      void Reset();

      /**
       * @brief Starts or resumes the stopwatch.
       *
       * @sideeffects Marks the stopwatch as running and starts the underlying stopwatch.
       */
      void Start();

      /**
       * @brief Stops the stopwatch.
       *
       * @sideeffects Marks the stopwatch as not running and stops the underlying stopwatch.
       */
      void Stop();

    protected:
      std::unique_ptr<Poco::Stopwatch> m_pStopwatch;///< Owning pointer to the underlying `Poco::Stopwatch` instance.
      bool m_bRunning = false;///< Logical running state maintained by this wrapper.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
