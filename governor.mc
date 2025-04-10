; // Event header

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
               Runtime=0x1:FACILITY_RUNTIME
              )

LanguageNames=(English=0x409:MSG00409)

; // The following are the categories of events.

MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=NETWORK_CATEGORY
Language=English
Network Events
.

MessageId=0x2
SymbolicName=FILE_CATEGORY
Language=English
File Events
.

MessageId=0x3
SymbolicName=PROCESS_CATEGORY
Language=English
Process Events
.

MessageId=0x4
SymbolicName=REGISTRY_CATEGORY
Language=English
Registry Events
.

MessageId=0x5
SymbolicName=REPORTING_CATEGORY
Language=English
ALCA Reporting Events
.

MessageId=0x6
SymbolicName=MAIN_CATEGORY
Language=English
Service Events
.

; // The following are the message definitions.

MessageIdTypedef=DWORD

MessageId=0x100
Severity=Error
Facility=System
SymbolicName=MSG_SYSTEM_ERROR
Language=English
%1 failed: %2
.

MessageId=0x101
Severity=Error
Facility=Runtime
SymbolicName=MSG_RUNTIME_ERROR
Language=English
Function %1: %2
.

MessageId=0x102
Severity=Warning
Facility=Runtime
SymbolicName=MSG_RUNTIME_WARN
Language=English
Function %1: %2
.

MessageId=0x103
Severity=Informational
Facility=Runtime
SymbolicName=MSG_RUNTIME_INFO
Language=English
%1
.

MessageId=0x104
Severity=Success
Facility=Runtime
SymbolicName=MSG_RUNTIME_SUCCESS
Language=English
%1
.
