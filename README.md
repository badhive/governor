## Governor

Governor is the flagship Windows Sensor for the [ALCA Engine](https://github.com/badhive/alca).

### How it works

A sensor is a telemetry source that provides ALCA with events for rules to scan. In Governor's case, most
events come directly from Event Tracing for Windows (ETW) and are transferred to ALCA via plain sockets, using 
[FlatBuffers](https://github.com/google/flatbuffers) for formatting.

The session created by Governor is a NT Kernel session, ensuring malware analysed by it cannot prevent these events
from being published without kernel access.

### Installation and Usage

Governor can be installed as a service or run as a standalone binary. It requires Administrator privileges to run.

To build, open the solution (.sln) in Visual Studio 2022 and build the Governor project.

```
C:\> .\x64\Debug\governor.exe /?

ALCA: Governor

ALCA Engine's flagship user-mode Windows Sensor.
Logs can be viewed in Event Viewer (Alca.Sensor.Governor)

Usage:

  install       : Install as service
      /run      : Run after installation
      /startup  : Configure service to start automatically on boot

  uninstall     : Uninstall service

  console       : Run as console application
      /verbose  : Log verbosely
```

Logs can be viewed in the Event Viewer (service mode) or the console (console mode).
