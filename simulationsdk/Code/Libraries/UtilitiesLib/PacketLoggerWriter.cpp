//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketLoggerWriter.cpp
 * @brief Implements writing of packet log files from buffers.
 *
 * This file provides the implementation of the CPacketLoggerWriter class, which allows
 * writing frames and buffer data to a packet log file. Supports beginning and ending frames,
 * and writing buffer data for each frame.
 *
 * @see sbio::utils::CPacketLoggerWriter
 */
#include "PacketLoggerWriter.h"
#include "PacketLoggerTypes.h"
#include <iostream>
#include <cstdio>

using namespace std;
using namespace sbio::utils;

// Constructs a writer for the specified packet log file.
CPacketLoggerWriter::CPacketLoggerWriter(const std::filesystem::path& filePath)
{
  std::string sFile = filePath.string();
  std::remove(sFile.c_str());

  m_hFile = fopen(sFile.c_str(), "wb");
  if (m_hFile != nullptr)
  {
    cout << "Writing to packet log file " << sFile << endl;
  }
}

// Destructor. Closes the log file.
CPacketLoggerWriter::~CPacketLoggerWriter()
{
  if (m_hFile != nullptr)
  {
    fclose(m_hFile);
  }
}

// Begins a new frame in the log file.
void CPacketLoggerWriter::BeginFrame()
{
  if (m_hFile == nullptr)
  {
    return;
  }

  SBeginFrame frame;
  frame.nFrame = m_nFrame;
  if (!WritePacketLogData(m_hFile, &frame, sizeof(SBeginFrame), 1))
  {
    ClosePacketLogFileOnFailure(m_hFile);
  }
}

void CPacketLoggerWriter::ClosePacketLogFileOnFailure(FILE*& hFile)
{
  if (hFile != nullptr)
  {
    fclose(hFile);
    hFile = nullptr;
  }
}

// Writes a buffer to the current frame in the log file.
void CPacketLoggerWriter::WriteBuffer(const char* buffer, unsigned int nSize)
{
  // If the file handle is null, the log file is not open and cannot be written.
  if (m_hFile == nullptr)
  {
    return;
  }

  // Validate the buffer and size parameters. A null buffer with a non-zero size is invalid.
  if (buffer == nullptr && nSize > 0)
  {
    return;
  }

  // Write the frame buffer header, which includes the size of the buffer.
  SFrameBuffer frameBuffer;
  frameBuffer.nFrameBufferSize = nSize;

  // If writing the frame buffer header fails, close the log file and return.
  if (!WritePacketLogData(m_hFile, &frameBuffer, sizeof(SFrameBuffer), 1))
  {
    ClosePacketLogFileOnFailure(m_hFile);
    return;
  }

  // If the buffer size is zero, there is no buffer data to write, so return after writing the header.
  if (nSize == 0)
  {
    return;
  }

  // Write the buffer data to the log file. If writing fails, close the log file.
  if (!WritePacketLogData(m_hFile, buffer, sizeof(char), nSize))
  {
    ClosePacketLogFileOnFailure(m_hFile);
  }
}

bool CPacketLoggerWriter::WritePacketLogData(FILE* hFile, const void* pBuffer, size_t nElementSize, size_t nElementCount)
{
  // Validate the file handle and buffer parameters. A null file handle or a null buffer with a non-zero element count is invalid.
  if (fwrite(pBuffer, nElementSize, nElementCount, hFile) != nElementCount)
  {
    return false;
  }

  return fflush(hFile) == 0;
}

// Ends the current frame in the log file.
void CPacketLoggerWriter::EndFrame()
{
  if (m_hFile == nullptr)
  {
    return;
  }

  // Write the end frame record to the log file. If writing fails, close the log file and return.
  SEndFrame frame;
  if (!WritePacketLogData(m_hFile, &frame, sizeof(SEndFrame), 1))
  {
    ClosePacketLogFileOnFailure(m_hFile);
    return;
  }

  ++m_nFrame;
  cout << m_nFrame << endl;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
