**Modified IPC Build Configuration README**

### Introduction
This README provides instructions for configuring the build environment for the IPC example ex02 on the DRA7xx platform. The setup involves replacing both the host and shared folders with the provided ones, ensuring to back up the original folders safely.

### Instructions
Follow these steps to configure the build environment:

1. **Backup original folders**:
> **IMPORTANT:** Before making any changes, create backups of both the original host and shared folders to ensure you can revert if needed.

2. **Locate the provided host and shared folders**: 
    - Find the host and shared folders provided for this configuration.

3. **Replace the host and shared folders**:
    - Replace the existing host and shared folders in the following location:
      ```
      /home/administrator/processor_sdk_rtos_am57xx_08_01_00_09/ipc_3_50_04_08/examples/DRA7XX_linux_elf/ex02_messageq
      ```
      with the provided host and shared folders.

### Final Configuration
After completing the above steps, your directory structure should have replaced the original folders with the provided ones.
```
processor_sdk_rtos_am57xx_08_01_00_09/
│
└───ipc_3_50_04_08/
    │
    └───examples/
        │
        └───DRA7XX_linux_elf/
            │
            └───ex02_messageq/
                │   
                └───host/                  <-- Provided host folder
                │   │
                │   └───(host files and directories)
                │   
                └───shared/                <-- Provided shared folder
                │   │
                │   └───(shared files and directories)
                │
                └───(other files and directories)
```

### Additional Notes
- Ensure that all necessary dependencies and libraries are installed as per the project requirements.
- Verify the permissions and access rights of the replaced folders to avoid any issues during the build process.
- Ensure all required paths are properly exported before running `make`.
- Safely store the backups of the original folders in case they are needed in the future.

### Building the Project
To build the project, navigate to the `host` folder and run the following command:
```bash
make
```
Or Import the project in CCS as C/C++ Existing Code as Makefile Project and hit 'Build'
<p align="center">
  <img src="https://github.com/eSpark-Consultants/ilanganor-firmwaream571x/assets/143803999/37e6db82-b716-4e30-98d1-d88ca84140a7" alt="Screenshot" width="400" />
</p>

