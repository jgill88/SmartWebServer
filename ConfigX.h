// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStep SmartWebServer infrequently used eXtended options

//                                 *** Most users will not change anything in this file! ***

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified SERIAL_DEBUG port
// Note that the SERIAL_DEBUG port cannot be used for normal communication with OnStep

//      Parameter Name              Value        Default  Notes                                                                 Hint
#define DEBUG                         OFF //         OFF, Use ON for background error messages only, use VERBOSE for all      Infreq
                                          //              error and status messages, use PROFILER for VT100 task profiler,
                                          //              use REMOTE to forward debug output to OnStep
#define SERIAL_DEBUG               Serial //      Serial, Or use any h/w serial port. Serial1 or Serial2, etc. as supported.  Option
#define SERIAL_DEBUG_BAUD            9600 //        9600, n. Where n=9600,19200,57600,115200 (common baud rates.)             Option

// STATUS LED ----------------------------------------------------------------------------------------------------------------------
#define LED_STATUS_ON_STATE          HIGH //        HIGH, Use LOW so indicator stays on when connected if needed.             Option

// IP ADDRESS SETTINGS -------------------------------------------------------------------------------------------------------------
// For Wifi these settings are stored in NV (EEPROM) and will not revert to defaults first set from these values unless
// NV is wiped, these settings can be changed at runtime however.  For Ethernet, settings must be changed below to take effect
#define IP_ADDRESS         (192,168,1,50) //      ip(,,), Wifi Station/Ethernet default IP Address                            Adjust
#define DNS_ADDRESS         (192,168,1,1) //   myDns(,,), Wifi Station/Ethernet default DNS Server Address                    Adjust
#define GATEWAY_ADDRESS     (192,168,1,1) // gateway(,,), Wifi Station/Ethernet default GATEWAY Address                       Adjust
#define SUBNET            (255,255,255,0) //  subnet(,,), Wifi Station/Ethernet default SUBNET                                Adjust

#define IP_ADDRESS_AP       (192,168,0,1) //      ip(,,), Wifi Access Point default IP Address                                Adjust
#define GATEWAY_AP          (192,168,0,1) // gateway(,,), Wifi Access Point default GATEWAY Address                           Adjust
#define SUBNET_AP         (255,255,255,0) //  subnet(,,), Wifi Access Point default SUBNET                                    Adjust

// PASSWORD ------------------------------------------------------------------------------------------------------------------------
#define Default_Password       "password" //  "password", Adjust as required, this can be changed at runtime also.            Option

// NON-VOLATILE STORAGE ------------------------------------------------------------------------------------------------------------
#define NV_DEFAULT                     ON //          ON, Uses HAL specified default, for remembering settings when           Infreq
                                          //              powered off.  This is usually done using the microcontrollers
                                          //              built-in EEPROM or FLASH emulation of EEPROM.
