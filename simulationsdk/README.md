# SimulationSDK
The SimulationSDK contains application and libraries developed by SimBlocks LLC. 

## Purpose

This repository contains C++ code for building and running the SimBlocks CIGI Host Emulator and image-generator test applications. It provides reusable simulation middleware libraries plus sample applications for exercising Common Image Generator Interface (CIGI) communication between a host and an image generator.
CIGI versions 3.3 and 4.0 are supported.

Version 4.0 has been approved by the Simulation Interoperability Standards Organization (SISO). 
- https://www.sisostandards.org/page/StandardsProducts
- https://www.sisostandards.org/resource/resmgr/standards_products/siso-std-013-2014_standard_f.pdf

## Prerequisites  
- Windows 11
- Microsoft Visual Studio Professional 2022

# Setup
Copy thirdparty DLLs
1. Navigate to **simulationsdk**
2. Run **copy_DLLs_to_SimulationSDK_Release.bat**

SISO Enumerations
1. Go to https://www.sisostandards.org/page/ReferenceDocuments
2. Download the latest **SISO-REF-010 zip**
3. Extract the zip contents to **Data\Libraries\EntityLib\**

## Build  
1. Open **Simulation SDK - CIGI.sln** in Visual Studio 2022
2. Select the **vs2022_Release** Configuration and **x64** Platform. 
3. Build the Solution.

## Running the Tools
1. In Visual Studio, right-click on the **HostEmulator** project and select **Set as Startup Project**
2. In Visual Studio, right-click on **CIGITestImageGenerator** -> Debug -> Start New Instance



