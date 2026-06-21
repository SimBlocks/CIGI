//Copyright SimBlocks LLC 2016-2026
/**
 * @file Task.cpp
 * @brief Implements a wrapper for Poco::Task with new task management support.
 *
 * This file provides the implementation of the CTask class, which extends Poco::Task
 * and provides a container for managing new tasks that will be passed to smart pointers
 * when run.
 *
 * @see sbio::utils::CTask
 */
#include "Task.h"

using namespace sbio::utils;

// Constructs a task with the specified name.
CTask::CTask(const std::string& name) : Task(name)
{
}

// Destructor. Cleans up task resources.
CTask::~CTask()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
