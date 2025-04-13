## Governor

Governor is the flagship Windows Sensor for the [ALCA Engine](https://github.com/badhive/alca).

### How it works

A sensor is a telemetry source that provides ALCA with events for rules to scan. In Governor's case, most
events come directly from Event Tracing for Windows (ETW) and are transferred to ALCA via plain sockets, using 
[FlatBuffers](https://github.com/google/flatbuffers) for formatting.

The session created by Governor is a NT Kernel session, and as such requires administrator privileges to use.

### Installation and Usage

Governor can be installed as a service or run as a standalone binary.

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

The available fields for each event (\*Event) and their types can be found in the [ALCA FlatBuffers schema file](alca.fbs).

### Extending

Extensions to Governor are always welcome, and requests for new fields can be opened in the 
[Issues](https://github.com/badhive/governor/issues) page. As Governor is natively supported by ALCA, you would likely 
need to submit two PRs if you want to add extensions yourself - one to [ALCA](https://github.com/badhive/alca) and one here.
Currently, extensions are accepted as long as they only involve adding new fields, to ensure backwards compatibility.

Each PR must include:
1. the fields you wish to add to the event type(s)
2. serialisation / deserialisation of these new fields
3. the updated event type schema (alca.fbs) and associated header (alca.fb.hpp).

Depending on the volume, newly introduced fields should result in a minor version update to both ALCA and Governor.

### Limitations (entirely ETW's fault)

- On occasion, certain file events (`FileRename`, `FileModify`) may not be caught by Governor. This
  is due to unknown behaviour within ETW itself, perhaps related to some optimisation. Most runs, however,
  will register these events. Keep this in mind when writing or running alca rules.
- ETW provides a very limited amount of information for registry events, on top of having no way to determine what
  what events affect which parent keys / subkeys. As such, registry key names and value names
  are provided in separate events as they cannot be correlated (brilliant!). IOCs related to the registry will likely
  have to revolve around unique registry key names or value names to reduce false positives. There currently isn't a way to get 
  more information without a registry filter driver.

### Disclaimer

Under absolutely NO circumstances must you run Governor on a production machine. It relies on being able to download and execute
remote binaries, and comes with no authentication or encryption. You would be shooting yourself in the foot doing so, and I will not be
liable for any damage caused.
