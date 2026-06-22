# Overview
Common Image Generator Interface (CIGI) Middleware and Tools

## SimBlocks CIGI Host Emulator
Individual CIGI packets may be sent.
![simulationsdk/screenshots/Host Emulator - IG Control.png](https://github.com/SimBlocks/CIGI/blob/main/screenshots/Host%20Emulator%20-%20IG%20Control.png))

Scripts may be created to send sequences of CIGI packets.
![Host Emulator - Scripts - Entity Position Translate](https://github.com/SimBlocks/CIGI/blob/main/screenshots/Host%20Emulator%20-%20Scripts%20-%20Entity%20Position%20-%20Translate.png)

## CIGI Test Image Generator

The test IG displays middleware message content. 
![CIGI Test Image Generator](https://github.com/SimBlocks/CIGI/blob/main/screenshots/Test%20Image%20Generator.png)

All CIGI message content is logged to **simulationsdk\Data\Applications\CIGI ImageGenerator\IGMessageLog.log**
![CIGITestImageGenerator - Messagee Log](https://github.com/SimBlocks/CIGI/blob/main/screenshots/IGMessageLog.log.png)

# Prerequisites  
- Windows 11
- Install CMake from https://cmake.org/download/
- Microsoft Visual Studio Professional 2022
- Visual Studio Installer - Individual Components:  
 MSVC v143 - vs 2022 C++ x64/x86 build tools (v14.44-17.14)

# Setup 
1. Clone the repo  
*Prefer cloning the repo rather than downloading the zip in order for submodule initialization to work.*

2. Initialize Submodules
- a. Open a command prompt
- b. Navigate to the cloned **CIGI** folder
- c. type **git submodule update --init --recursive**
- d. Wait for the submodules to finish downloading
  
3. Follow instructions in [thirdparty/README.md](https://github.com/SimBlocks/thirdparty/blob/main/README.md)
4. Follow instructions in [simulationsdk/README.md](simulationsdk/README.md)


# License
- See [thirdparty/LICENSE.md](https://github.com/SimBlocks/thirdparty/blob/main/LICENSE)
- See [LICENSE](LICENSE)

# Support
For feature requests and to report issues for this product, please utilize the **GitHub Issues** page.

For more custimized software development support, please reach out to our team at **sales@simblocks.io**

For general CIGI questions, please contact the CIGI Product Support Group at https://sisostandards.connectedcommunity.org/home
