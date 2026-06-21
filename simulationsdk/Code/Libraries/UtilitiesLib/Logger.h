//Copyright SimBlocks LLC 2016-2026
/**
 * @file Logger.h
 * @brief Declares logging interfaces and classes for message output.
 *
 * Defines the ILogger interface for logging messages at various levels and the CLogger
 * class for concrete logging implementations. Supports logging to console and file, and
 * integrates with Poco::Logger for advanced logging features.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LOGGER_H
#define SIMBLOCKS_UTILITIES_LOGGER_H

#include <string>
#include <sstream>

namespace Poco
{
  class Logger;
}

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Log severity levels understood by `CLogger::Log()` and the constructor.
     */
    enum class ELogLevel
    {
      LOG_UNKNOWN,///< Unrecognized level. `CLogger::Log()` performs no action for this value.
      LOG_ERROR,///< Error messages.
      LOG_WARNING,///< Warning messages.
      LOG_INFORMATION,///< Informational messages.
      LOG_DEBUG,///< Debug messages.
    };

    /**
     * @brief Abstract logging sink interface.
     *
     * Implementations receive formatted text for the supported severity levels.
     * The interface does not prescribe storage, synchronization, or exception behavior.
     */
    struct ILogger
    {
    public:
      /**
       * @brief Destroys the logger interface.
       */
      virtual ~ILogger() {};

      /**
       * @brief Logs a debug message.
       * @param sText Message text to log.
       */
      virtual void LogDebug(const std::string& sText) const = 0;
      /**
       * @brief Logs an error message from a stream buffer.
       * @param stream Stream whose contents are converted to text and logged.
       */
      virtual void LogError(const std::stringstream& stream) const = 0;
      /**
       * @brief Logs an error message.
       * @param sText Message text to log.
       */
      virtual void LogError(const std::string& sText) const = 0;
      /**
       * @brief Logs an informational message from a stream buffer.
       * @param stream Stream whose contents are converted to text and logged.
       */
      virtual void LogInformation(const std::stringstream& stream) const = 0;
      /**
       * @brief Logs an informational message.
       * @param sText Message text to log.
       */
      virtual void LogInformation(const std::string& sText) const = 0;
      /**
       * @brief Logs a warning message.
       * @param sText Message text to log.
       */
      virtual void LogWarning(const std::string& sText) const = 0;
    };

    /**
     * @brief `ILogger` implementation backed by `Poco::Logger`.
     *
     * `CLogger` can write to the console, to a log file named `<requested-name>.log`, or to both.
     * The logger uses the requested public name directly as the underlying Poco logger name.
     *
     * @invariant `m_sName` stores the public logger name requested at construction.
     * @ownership `m_pLogger` is a non-owning pointer to a Poco-managed logger instance.
     */
    class CLogger : public ILogger
    {
    public:
      /**
       * @brief Creates a logger instance.
       * @param sName Public logger name and the base file name used when file logging is enabled.
       * @param bLogToConsole `true` to attach a console output channel.
       * @param bLogToFile `true` to attach a file output channel targeting `<sName>.log`.
       * @param eLogLevel Minimum log priority for the underlying Poco logger.
       *
       * When file logging is enabled, any existing log file of the same name is removed before logging starts.
       *
       * @sideeffects May create or truncate `<sName>.log` in the current working directory.
       */
      CLogger(const std::string& sName, bool bLogToConsole = false, bool bLogToFile = true, ELogLevel eLogLevel = ELogLevel::LOG_ERROR);
      /**
       * @brief Constructs a `CLogger` instance.
       */
      CLogger(const CLogger&) = delete;
      CLogger& operator=(const CLogger&) = delete;
      /**
       * @brief Constructs a `CLogger` instance.
       * @param rhs Rhs value.
       */
      CLogger(CLogger&& rhs) = delete;
      CLogger& operator=(CLogger&& rhs) = delete;

      /**
       * @brief Destroys the logger instance.
       *
       * Releases this object's reference to the underlying Poco logger.
       */
      virtual ~CLogger();

      /**
       * @brief Logs a message at the specified severity.
       * @param eLogLevel Severity selector.
       * @param sText Message text to log.
       *
       * Dispatches to the corresponding severity-specific method. `LOG_UNKNOWN` is ignored.
       *
       * @sideeffects Writes to the configured logger sinks when the level is recognized.
       */
      void Log(ELogLevel eLogLevel, const std::string& sText) const;

      /**
       * @brief Logs a debug message.
       * @param sText Message text to log.
       *
       * @failurecases `Poco::Exception` instances are caught and suppressed.
       */
      virtual void LogDebug(const std::string& sText) const override;
      /**
       * @brief Logs an error message from a stream buffer.
       * @param stream Stream whose contents are converted to text and logged.
       *
       * @failurecases Exceptions from the underlying string overload are suppressed there.
       */
      virtual void LogError(const std::stringstream& stream) const override;
      /**
       * @brief Logs an error message.
       * @param sText Message text to log.
       *
       * The implementation prefixes the text with `"Error: "`.
       *
       * @failurecases `Poco::Exception` instances are caught and suppressed.
       */
      virtual void LogError(const std::string& sText) const override;
      /**
       * @brief Logs an informational message from a stream buffer.
       * @param stream Stream whose contents are converted to text and logged.
       *
       * @failurecases `Poco::Exception` instances raised while converting or forwarding are caught and suppressed.
       */
      virtual void LogInformation(const std::stringstream& stream) const override;
      /**
       * @brief Logs an informational message.
       * @param sText Message text to log.
       *
       * @failurecases `Poco::Exception` instances are caught and suppressed.
       */
      virtual void LogInformation(const std::string& sText) const override;
      /**
       * @brief Logs a warning message.
       * @param sText Message text to log.
       *
       * The implementation prefixes the text with `"Warning: "`.
       *
       * @failurecases `Poco::Exception` instances are caught and suppressed.
       */
      virtual void LogWarning(const std::string& sText) const override;

    private:
      Poco::Logger* m_pLogger;///< Non-owning pointer to the underlying Poco logger instance created for this object.
      std::string m_sName;///< Public logger name used for file naming.
      bool m_bLogToFile;///< `true` when this logger writes to `<m_sName>.log`.
    };
  }
}
#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
