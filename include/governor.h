 // Event header
 // The following are the categories of events.
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_RUNTIME                 0x1


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: NETWORK_CATEGORY
//
// MessageText:
//
// Network Events
//
#define NETWORK_CATEGORY                 ((WORD)0x00000001L)

//
// MessageId: FILE_CATEGORY
//
// MessageText:
//
// File Events
//
#define FILE_CATEGORY                    ((WORD)0x00000002L)

//
// MessageId: PROCESS_CATEGORY
//
// MessageText:
//
// Process Events
//
#define PROCESS_CATEGORY                 ((WORD)0x00000003L)

//
// MessageId: REGISTRY_CATEGORY
//
// MessageText:
//
// Registry Events
//
#define REGISTRY_CATEGORY                ((WORD)0x00000004L)

//
// MessageId: REPORTING_CATEGORY
//
// MessageText:
//
// ALCA Reporting Events
//
#define REPORTING_CATEGORY               ((WORD)0x00000005L)

//
// MessageId: MAIN_CATEGORY
//
// MessageText:
//
// Service Events
//
#define MAIN_CATEGORY                    ((WORD)0x00000006L)

 // The following are the message definitions.
//
// MessageId: MSG_SYSTEM_ERROR
//
// MessageText:
//
// %1 failed: %2
//
#define MSG_SYSTEM_ERROR                 ((DWORD)0xC0000100L)

//
// MessageId: MSG_RUNTIME_ERROR
//
// MessageText:
//
// Function %1: %2
//
#define MSG_RUNTIME_ERROR                ((DWORD)0xC0010101L)

//
// MessageId: MSG_RUNTIME_WARN
//
// MessageText:
//
// Function %1: %2
//
#define MSG_RUNTIME_WARN                 ((DWORD)0x80010102L)

//
// MessageId: MSG_RUNTIME_INFO
//
// MessageText:
//
// %1
//
#define MSG_RUNTIME_INFO                 ((DWORD)0x40010103L)

//
// MessageId: MSG_RUNTIME_SUCCESS
//
// MessageText:
//
// %1
//
#define MSG_RUNTIME_SUCCESS              ((DWORD)0x00010104L)

