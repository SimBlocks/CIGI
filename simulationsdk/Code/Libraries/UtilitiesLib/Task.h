//Copyright SimBlocks LLC 2016-2026
/**
 * @file Task.h
 * @brief Declares a wrapper class for Poco::Task with support for new task management.
 *
 * Defines the CTask class, which extends Poco::Task and provides a container for
 * managing new tasks that will be passed to smart pointers when run.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_TASK_H
#define SIMBLOCKS_UTILITIES_TASK_H

#include "Poco/Task.h"
#include <memory>
#include <vector>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Thin `Poco::Task` derivative with storage for additional task pointers.
     *
     * `CTask` adds `m_NewTasks`, a public container of raw `CTask*` values. The class itself does not
     * delete the pointers stored in that container; ownership must be managed by the code that populates
     * and consumes it.
     */
    class CTask : public Poco::Task
    {
    public:
      std::vector<CTask*> m_NewTasks;///< Raw pointers to additional tasks. `CTask` does not destroy these pointers in its destructor.

      /**
       * @brief Constructs a task with the specified name.
       * @param name Task name passed to the `Poco::Task` base class.
       */
      CTask(const std::string& name);

      /**
       * @brief Destroys the task wrapper.
       *
       * The destructor performs no additional cleanup beyond base-class destruction.
       */
      ~CTask() override;
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
