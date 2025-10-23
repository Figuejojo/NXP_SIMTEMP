# Simulated Temperature Sensor Driver

## Author
- **Name:** Jose Jorge Figueroa  
- **Role:** Embedded Systems Engineer  
- **Contact:** [GitHub](https://github.com/Figuejojo)  
---
# Design
This project builds a small system that simulates a hardware sensor in the Linux kernel and exposes it to user space, with a user-space application to configure and read it.  
A lightweight GUI is **optional**.

## Why This Matters
In many embedded or Linux-based projects, real hardware sensors may not always be available during early development or testing.  
By simulating a sensor inside the kernel, we can:  
- **Develop and test drivers** without relying on physical hardware.  
- **Validate user-space applications** that consume sensor data.  
- **Enable CI/automation** environments where hardware cannot be easily deployed.  
- **Experiment with kernel-user interfaces** (character devices, sysfs, ioctl, polling).  
This makes the project valuable for learning, prototyping, and validating system designs before integrating actual hardware.

## Goals & Approach
### Primary Objectives
- **Incremental development** with time constraints — Agile, results-driven delivery
- **Stable baseline** before implementing optional features
- **Code quality** ensured through early integration of linting tools
- **Comprehensive testing** at all levels of the stack

### Development Methodology
- **Test-Driven Development (TDD)** where practical
- **Continuous Integration** with automated testing
- **Documentation-driven development** to maintain clarity

### Quality Assurance
- **lint.sh** integrated early to ensure code quality from the start
- **Static analysis** for both kernel and userspace code

## System Architecture

### Kernel ↔ User Space Interface (Overview)
The diagram below shows how the simulated hardware (a temperature sensor) is integrated through the driver and exposed to user space.

```mermaid
flowchart TB
    subgraph Hardware
        A["Temperature Sensor 
        (Simulated)"]
    end

    subgraph "Kernel Space" 
        B["NXPSimTemp
        (Character Driver)"]
    end

    C["/dev/simtemp/"]
    D["/sys/class/simtemp"]
    E["ioctl"]
    F["cat,echo"]
    G["open, read, write"]

    A --> B

    B --> C
    B --> D

    subgraph User Space
        C --> E 
        C --> F
        C --> G

        D --> F
        D --> G
    end
```
### Kernel Space
The driver is responsible for simulating the temperature sensor behavior (e.g., Gaussian noise, ramp, normal) and handling requests from user space.
#### Simmulation Engine
- **Normal Mode**:  Stable temperature readings with minimal variation.
- **Nisy Mode**: Gaussian noise applied to simulate real sensor behavior.
- **Ramp Mode**: Linear temperature changes for testing threshold detection.
#### Configuration Management
- Parameter validation for all user-configurable settings
- Mutex and Atomic updates to prevent race conditions

#### Event System
- Threshold detection and status reporting.
- Poll support for efficient event waiting.

### User Space 
The user space provides the interface for applications or users to issue requests and configure driver settings.

#### Character Device /dev/x
Represents the device node, which acts as the main data path for operations.
Supported operations include:
- open(): Initializes communication with the device.
- read(): Retrieves the latest simulated sensor value.
- write(): Sends commands or data to the device (if supported).
- poll(), epoll(): Enables event-driven data access.
- icotl() (Optional): Provides extended control operations.
- close(): Cleans up device resources.

#### /sys/class/x
The ```/sys/``` directory exposes configuration parameters through a simple file-based interface.
Available parameters in this module:
- sampletime_ms (RW): Update period
- threshold-mC  (RW): Alert Threshold in m°C
- mode          (RW): enum(Normal|Noisy|Ramp)
- stats         (RO): Counter.

## CLI Design
The CLI application follows a layered architecture.

### UML Architecture Design
```mermaid
classDiagram
    class NXPSimtempCLI {
        - kDefaultBase: const string
        - kDefaultDev: const string
        - attrMap: map~string, string~
        
        + NXPSimtempCLI()
        + execute(argc: int, argv: char**): int
        + setAttribute(att: string, value: string): int
        + getAttribute(att: string): int
        + pollSamples(numSamples: int): int
        + printHelp(): void
        
        - readFile(path: string): string
        - writeFile(path: string, value: string): void
        - getAttributePath(att: string): string
        - handleError(message: string, useErrno: bool): void
        - isValidAttribute(att: string): bool
        - isAttributeReadOnly(att: string): bool
        - executeSetCommand(args: vector~string~): int
        - executeGetCommand(args: vector~string~): int
        - executePollCommand(args: vector~string~): int
    }
    
    class SimtempSample {
        + timestamp_ns: uint64_t
        + temp_mC: int32_t
        + flags: uint32_t
        __attribute__((packed))
    }
    
    class FileSystem {
        <<external>>
        + /sys/class/misc/simtemp/
        + /dev/simtemp
    }
    
    class SystemCalls {
        <<external>>
        + open()
        + read()
        + write()
        + close()
    }
    
    %% Relationships
    NXPSimtempCLI --> SimtempSample : uses
    NXPSimtempCLI --> FileSystem : interacts with
    NXPSimtempCLI --> SystemCalls : depends on
    
    %% Composition for attribute map
    NXPSimtempCLI "1" *-- "1" AttributeMap : contains
    class AttributeMap {
        + SAMP → "sampling_ms"
        + THRS → "threshold_mC"
        + MODE → "mode"
        + STAT → "state"
    }
```
### Enhanced Sequence Diagrams

#### Set Command Squence Command (AI Generated)

``` mermaid
sequenceDiagram
    participant U as User
    participant M as Main
    participant C as NXPSimtempCLI
    participant V as Validator
    participant F as FileSystem
    participant D as Driver

    U->>M: nxp_simtempcli set MODE 1
    M->>C: execute(argc, argv)
    
    C->>C: executeSetCommand(args)
    C->>V: isValidAttribute("MODE")
    V-->>C: true
    C->>V: isAttributeReadOnly("MODE")
    V-->>C: false
    C->>F: getAttributePath("MODE")
    F-->>C: "/sys/class/misc/simtemp/mode"
    C->>F: writeFile(path, "1\n")
    F->>D: write to sysfs file
    D-->>F: success
    F-->>C: success
    C->>U: "MODE set to: 1"
```

#### Get Command Squence Command (AI Generated)
```mermaid
sequenceDiagram
    participant U as User
    participant M as Main
    participant C as NXPSimtempCLI
    participant V as Validator
    participant F as FileSystem
    participant D as Driver

    U->>M: nxp_simtempcli get SAMP
    M->>C: execute(argc, argv)
    
    C->>C: executeGetCommand(args)
    C->>V: isValidAttribute("SAMP")
    V-->>C: true
    C->>F: getAttributePath("SAMP")
    F-->>C: "/sys/class/misc/simtemp/sampling_ms"
    C->>F: readFile(path)
    F->>D: read from sysfs file
    D-->>F: "100"
    F-->>C: "100"
    C->>U: "SAMP: 100"
```

## Application (Optional)
A user-space application to:
- Read sensor values directly
- Modify configuration parameters
- (Optionally) GUI for visualization and control

## Future Enhancements Section
- Design and Develop a GUI for the Driver
- Modify Qemu so it is able to run custom dtb's.
- Support multiple driver simulations.
    - Move from single instance to multi-instance.
- Add Unit testing.
