//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketLoggerReader.cpp
 * @brief Implements reading of packet log files into buffers.
 *
 * This file provides the implementation of the CPacketLoggerReader class, which loads
 * a packet log file into memory and allows reading frames as lists of typed buffers.
 * Supports checking for available data and reading frames for replay or analysis.
 *
 * @see sbio::utils::CPacketLoggerReader
 */
#include "PacketLoggerReader.h"
#include "PacketLoggerTypes.h"
#include "UtilitiesLib/FileSystemUtilities.h"
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;
using namespace sbio::utils;

// Constructs a reader for the specified packet log file.
CPacketLoggerReader::CPacketLoggerReader(const std::filesystem::path& filePath)
{
  m_pFileContents = ReadBinaryFileContents(filePath);

  if (m_pFileContents)
  {
    cout << "Reading packet log file " << filePath << endl;
    m_pBuffer = m_pFileContents->GetBuffer();
  }
}

// Destructor. Releases file buffer resources.
CPacketLoggerReader::~CPacketLoggerReader()
{
}

// Checks if there is more data to read from the log file.
bool CPacketLoggerReader::HasData() const
{
  return m_pFileContents != nullptr && !m_pFileContents->IsEmpty() && m_pBuffer < (m_pFileContents->GetBuffer() + m_pFileContents->GetSize());
}

// Reads the next frame from the log file as a list of buffers.
std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> CPacketLoggerReader::ReadFrame()
{
  std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> buffers;

  // Validate that the file contents and buffer are available before attempting to read.
  if (m_pFileContents == nullptr || m_pBuffer == nullptr)
  {
    return buffers;
  }

  // Store the initial buffer position to allow resetting if the frame is incomplete.
  const char* const pFileBegin = m_pFileContents->GetBuffer();
  const char* const pFileEnd = pFileBegin + m_pFileContents->GetSize();
  char* pFrameStart = m_pBuffer;
  bool bFrameComplete = false;

  while (m_pBuffer < pFileEnd)
  {
    // Check that enough bytes remain to read a frame header before attempting to read it.
    const auto bytesRemaining = static_cast<size_t>(pFileEnd - m_pBuffer);
    if (bytesRemaining < sizeof(SFrameHeader))
    {
      m_pBuffer = const_cast<char*>(pFileEnd);
      break;
    }

    // Read the frame header to determine the type of frame record and how to process it.
    SFrameHeader frameHeader;
    memcpy(&frameHeader, m_pBuffer, sizeof(SFrameHeader));

    // Process the frame record based on its ID, ensuring that sufficient bytes remain for each expected record type.
    if (frameHeader.ID == EFrameID::BEGIN_FRAME)
    {
      // A BEGIN_FRAME record should be followed by an SBeginFrame structure containing the frame number.
      if (bytesRemaining < sizeof(SBeginFrame))
      {
        m_pBuffer = const_cast<char*>(pFileEnd);
        break;
      }

      // Read the SBeginFrame record and print the frame number to standard output.
      SBeginFrame frame;
      memcpy(&frame, m_pBuffer, sizeof(SBeginFrame));
      m_pBuffer += sizeof(SBeginFrame);
      cout << frame.nFrame << endl;
    }
    else if (frameHeader.ID == EFrameID::FRAME_BUFFER)
    {
      // A FRAME_BUFFER record should be followed by an SFrameBuffer structure containing the buffer size, and then the buffer data itself.
      if (bytesRemaining < sizeof(SFrameBuffer))
      {
        m_pBuffer = const_cast<char*>(pFileEnd);
        break;
      }

      // Read the SFrameBuffer record to determine the size of the buffer data that follows,
      // and validate that enough bytes remain to read it.
      SFrameBuffer frameBuffer;
      memcpy(&frameBuffer, m_pBuffer, sizeof(SFrameBuffer));
      m_pBuffer += sizeof(SFrameBuffer);

      // Validate that the declared buffer size does not exceed the remaining bytes in the file or the maximum value for an int.
      const auto payloadBytesRemaining = static_cast<size_t>(pFileEnd - m_pBuffer);
      const auto frameBufferSize = frameBuffer.nFrameBufferSize;
      if (frameBufferSize > static_cast<uint64_t>(payloadBytesRemaining) || frameBufferSize > static_cast<uint64_t>(std::numeric_limits<int>::max()))
      {
        m_pBuffer = const_cast<char*>(pFileEnd);
        break;
      }

      // Allocate a new buffer for the frame data, copy the data from the file buffer, and advance the read cursor.
      const int bufferSize = static_cast<int>(frameBufferSize);
      std::unique_ptr<TBuffer<char>> pBuffer = std::make_unique<TBuffer<char>>(bufferSize);
      memcpy(pBuffer->GetBuffer(), m_pBuffer, static_cast<size_t>(bufferSize));
      m_pBuffer += bufferSize;

      buffers.push_back(std::move(pBuffer));
    }
    else if (frameHeader.ID == EFrameID::END_FRAME)
    {
      // An END_FRAME record should be followed by an SEndFrame structure. This marks the end of the current frame.
      if (bytesRemaining < sizeof(SEndFrame))
      {
        m_pBuffer = const_cast<char*>(pFileEnd);
        break;
      }

      // Read the SEndFrame record and mark the frame as complete to return the collected buffers.
      SEndFrame frame;
      memcpy(&frame, m_pBuffer, sizeof(SEndFrame));
      m_pBuffer += sizeof(SEndFrame);
      bFrameComplete = true;
      break;
    }
    else
    {
      // Encountering an unknown frame ID is treated as a malformed record, causing the reader to advance to end-of-file.
      m_pBuffer = const_cast<char*>(pFileEnd);
      break;
    }
  }

  // If the frame was not completed due to malformed or incomplete data,
  // reset the buffer position to the start of the frame and return an empty list.
  if (!bFrameComplete)
  {
    buffers.clear();
    if (m_pBuffer < pFileEnd)
    {
      m_pBuffer = pFrameStart;
    }
  }

  // If the end of the file has been reached, print a status line to standard output.
  if (m_pBuffer >= pFileEnd)
  {
    cout << "Finished reading packet log file " << endl;
  }

  return buffers;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
