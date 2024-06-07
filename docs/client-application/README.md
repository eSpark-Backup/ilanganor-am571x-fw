# AM57x Audio & DSP Test Project Build Overview

## Getting Started

These instructions will guide you through setting up both the audio and DSP test projects on your local machine for development and testing purposes.

### Prerequisites

Ensure you have the following prerequisites installed or available on your machine:

- AM57x SDK Linux (ti-processor-sdk-linux-am57xx-evm-08_02_01_00)
- AM57x SDK RTOS (processor_sdk_rtos_am57xx_08_01_00_09)
- Separate BIOS (bios_5_42_01_09)
- ACE_Wrappers for DSP_test project

### ACE_Wrappers Setup for DSP Test Project

To set up ACE_Wrappers for the DSP_test project, follow these steps:

1. Download ACE from the [ACE_TAO releases page](https://github.com/DOCGroup/ACE_TAO/releases/tag/ACE%2BTAO-8_0_0).

2. Follow the detailed build instructions provided in the [Building and Installing ACE on UNIX](https://www.dre.vanderbilt.edu/~schmidt/DOC_ROOT/ACE/ACE-INSTALL.html) documentation.

By completing these steps, you should have ACE_Wrappers set up and ready for use with the DSP_test project.

### Audio Test Project Setup

Follow these steps to compile the audio test project:

1. Clone the repository to your local machine using Git:

```bash
git clone https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/tree/modify-ipc-messageq
```

2. Open Code Composer Studio (CCS).

3. Import the Audio_test project as a CCS Project.

4. Ensure that the required paths are added correctly.
   - **Audio_test include path**:
     ![Audio_test_includepath0](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/3b6ea47c-ec5c-4c26-a219-ebcdc945bc85)
   - **Audio_test include path extended**:
     ![Audio_test-includepaths1](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/aeec3aa4-36da-4052-8d56-bbcd3f303c69)
   - **Audio_test_search path**:
     ![Audio_test_searchPath1](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/e9eda211-7b1a-4a5a-bfee-fff565baeebc)
   - **Audio_test_search path extended**:
     ![Audio_test_searchPath2](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/f267547f-dbba-473d-8eab-af6a78ec9c24)

By following these steps, you should be able to successfully set up and compile the audio test project for further development and testing. If you encounter any issues, refer to the troubleshooting section or contact the project maintainers for assistance.

### DSP Test Project Setup

Follow these steps to compile the DSP test project:

1. Clone the repository to your local machine using Git:

```bash
git clone https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/tree/modify-ipc-messageq
```

2. Open Code Composer Studio (CCS).

3. Import the DSP_test project as an existing project into the workspace.

4. Confirm that the correct paths are added as per the screenshots provided.
   - **Dsp-test-cross-compiler-setting**:
     ![Dsp-test-cross-compiler](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/a2870c24-0c2e-4d83-bf7d-5d46a781776e)
   - **dsp_test-includes**:
     ![ds_test-includes](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/e5ec97c8-85a6-439a-880d-2cc80966ff8e)
   - **lib-search-path**:
     ![lib-search-path](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/b2ad5174-cc3b-4e7e-9fd6-60fa64a160d2)
   - **shared-libs**:
     ![shared-libs](https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/2c132e57-820d-4cc0-b92c-b17dffa943cf)

By following these steps, you should be able to successfully set up and compile the DSP test project for further development and testing. If you encounter any issues, refer to the troubleshooting section or contact the project maintainers for assistance.
