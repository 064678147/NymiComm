/*
Nymi Communication Library

Copyright (c) 2014, Bionym Inc

Include this file in NEAs to access functions that allow interactions with a Nymi.
This library depends on BLE.
On Windows and Linux, you need a Bluegiga Ecodaemon.
On OS X and iOS, you need Core Bluetooth.
*/

/*!

\defgroup core Core
	\copybrief core
	\brief Concepts used throughout the library: initialization, termination, callbacks, and the error stream.
	\defgroup coreFunctions Core Functions
		Functions for concepts used throughout the library: initializing and terminating the NCL, setting the concurrency mode, adding callbacks, and reading errors.  
		\ingroup core
	\defgroup coreTypes Core Types
		Types for concepts used throughout the library: function return types, event types, error codes, and utility types.
		\ingroup core
	\defgroup coreConstants Core Constants
		Constants for concepts used throughout the library: special Nymi handle value for adding callbacks.
		\ingroup core

\defgroup negotiation Negotiation
	The Negotiation section documents provisioning and validation functionalities. Provisioning establishes a bridge between the  \ref glossaryNea "NEA" and a newly encountered Nymi,
	and validation allows the  \ref glossaryNea "NEA" to recognize a previously provisioned Nymi.
	\brief Necessary capabilities for negotiating with a Nymi: provisioning and validation.
	\defgroup negotiationFunctions Negotiation Functions
		Function used for negotiating with a Nymi.
		\ingroup negotiation
	\defgroup negotiationTypes Negotiation Types
		Types used for negotiating with a Nymi.
		\ingroup negotiation
	\defgroup negotiationConstants Negotiation Constants
		Constants used for negotiating with a Nymi.
		\ingroup negotiation

\defgroup streams Streams
	\copybrief streams
	\brief Streams a Nymi provides: ECG.
	\defgroup streamFunctions Stream Functions
		Functions for streaming.
		\ingroup streams
	\defgroup streamTypes Stream Types
		Types for streaming.
		\ingroup streams
	\defgroup streamConstants Stream Constants
		Constants for streaming.
		\ingroup streams

\defgroup security Security Tools
	\copybrief security
	\brief Security tools a Nymi provides: signatures, symmetric keys, and pseudorandom values.
	\defgroup securityFunctions Security Functions
		Functions for using the security tools a Nymi provides.
		\ingroup security
	\defgroup securityTypes Security Types
		Types for using the security tools a Nymi provides.
		\ingroup security
	\defgroup securityConstants Security Constants
		Constants for using the security tools a Nymi provides.
		\ingroup security

\defgroup misc Miscellaneous
	\copybrief misc
	\brief Other Nymi capabilities: Notification, firmware version, RSSI.
	\defgroup miscFunctions Miscellaneous Functions
		Functions for miscellaneous features.
		\ingroup misc
	\defgroup miscTypes Miscellaneous Types
		Types for miscellaneous features.
		\ingroup misc
	\defgroup miscConstants Miscellaneous Constants
		Constants for miscellaneous features.
		\ingroup misc
*/

#ifndef NCL_H_INCLUDED
#define NCL_H_INCLUDED

#include <stdio.h>

#if defined(_MSC_VER)
	#if defined(NCL_DLL_EXPORT)
		#define NCL __declspec(dllexport)
	#elif defined(NCL_STATIC)
		#define NCL
	#else
		#define NCL __declspec(dllimport)
	#endif
#elif defined(NCL_OSX)||defined(NCL_IOS)
	#define NCL __attribute__((visibility("default")))
#else
	#define NCL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Special value used in  \ref nclAddBehavior and  \ref nclRemoveBehavior to indicate any  \ref glossaryNymiHandle "Nymi handle". \ingroup coreConstants */
#define NCL_NYMI_HANDLE_ANY -1

/** \brief The number of LEDs on the  \ref glossaryNymi "Nymi". Used by  \ref NCL_EVENT_AGREEMENT. \ingroup negotiationConstants */
#define NCL_LEDS 5
/** \brief The number of LED patterns to be displayed during  \ref glossaryAgreement "agreement". Used by  \ref NCL_EVENT_AGREEMENT. \ingroup negotiationConstants */
#define NCL_AGREEMENT_PATTERNS 1

/** \brief Number of  \ref glossaryEcg "ECG" samples sent per  \ref NCL_EVENT_ECG  \ref NclEvent. \ingroup streamConstants */
#define NCL_ECG_SAMPLES_PER_EVENT 5

/*=====sizes=====*/
/** \brief The size of an  \ref NclProvisionId. \ingroup negotiationConstants */
#define NCL_PROVISION_ID_SIZE 16
/** \brief The size of an  \ref NclProvisionKey. \ingroup negotiationConstants */
#define NCL_PROVISION_KEY_SIZE 16

/** \brief The size of an  \ref NclPartnerPublicKey. \ingroup securityConstants */
#define NCL_PARTNER_PUBLIC_KEY_SIZE 64
/** \brief The size of an  \ref NclPartnerPrivateKey. \ingroup securityConstants */
#define NCL_PARTNER_PRIVATE_KEY_SIZE 32
/** \brief The size of an  \ref NclVkId. \ingroup securityConstants */
#define NCL_VK_ID_SIZE 16
/** \brief The size of an  \ref NclVk. \ingroup securityConstants */
#define NCL_VK_SIZE 64
/** \brief The size of an  \ref NclMessage. \ingroup securityConstants */
#define NCL_MESSAGE_SIZE 16
/** \brief The size of an  \ref NclSig. \ingroup securityConstants */
#define NCL_SIG_SIZE 64
/** \brief The size of an  \ref NclAdv. \ingroup securityConstants */
#define NCL_ADV_SIZE 16

/** \brief The size of an  \ref NclSkId. \ingroup securityConstants */
#define NCL_SK_ID_SIZE 16
/** \brief The size of an  \ref NclSk. \ingroup securityConstants */
#define NCL_SK_SIZE 16
/** \brief The size of an  \ref NclPrg. \ingroup securityConstants */
#define NCL_PRG_SIZE 16

/** \brief The size of an  \ref NclFirmwareVersion. \ingroup miscConstants */
#define NCL_FIRMWARE_VERSION_SIZE 20

/*=====enums=====*/
/** \brief NCL enum for Booleans. \ingroup coreTypes */
typedef enum{
	NCL_FALSE=0,/**< \brief false, 0 */
	NCL_TRUE=1/**< \brief true, 1 */
} NclBool;

/** \brief NCL modes, used in \ref nclInit. \ingroup coreTypes */
typedef enum{
	NCL_MODE_DEFAULT=0,/**< \brief Default run mode, with none of the following options enabled. */
	NCL_MODE_SYNCH=1,/**<\brief Callbacks will be called from inside \ref nclUpdate, which must be called regularly. If not set, callbacks happen when an event occurrs, on their own thread. */
	NCL_MODE_DEV=2/**< \brief Interact with dev mode Nymis, not normal Nymis. Dev mode Nymis are always authenticated. */
} NclMode;

/** \brief Types of events that can be  \ref glossaryTrigger "triggered". Used in the `.type` field of  \ref NclEvent. \ingroup coreTypes */
typedef enum{
	NCL_EVENT_ANY,/**< \brief A special value used in  \ref nclAddBehavior and  \ref nclRemoveBehavior to indicate any event. */
	NCL_EVENT_INIT,/**< \brief Library initialization has finished. Triggered by  \ref nclInit. Associated data will be stored in  \ref NclEvent::init. */
	NCL_EVENT_ERROR,/**< \brief Something has failed. No associated data inside the event, but the error code will have been set. See  \ref nclGetErrorCode. */
	NCL_EVENT_DISCOVERY,/**< \brief A provisioning Nymi has been \ref glossaryDiscovery "discovered". Enabled by  \ref nclStartDiscovery. Associated data will be stored in  \ref NclEvent::discovery. */
	NCL_EVENT_FIND,/**< \brief A specific provisioned Nymi has been \ref glossaryFinding "found". Enabled by  \ref nclStartFinding. Associated data will be stored in  \ref NclEvent::find. */
	NCL_EVENT_DETECTION,/**< \brief A Nymi has been \ref glossaryDetection "detected". Enabled by  \ref nclStartFinding. Associated data will be stored in  \ref NclEvent::detection. */
	NCL_EVENT_AGREEMENT,/**< \brief A shared secret has been agreed on by the Nymi and the NEA, and an LED pattern can now be shown to the user to ensure that the correct Nymi is being provisioned. Triggered by  \ref nclAgree. Associated data will be stored in  \ref NclEvent::agreement. */
	NCL_EVENT_PROVISION,/**< \brief A Nymi has been provisioned. Triggered by  \ref nclProvision. Associated data will be stored in  \ref NclEvent::provision. */
	NCL_EVENT_VALIDATION,/**< \brief A Nymi has been validated. Triggered by  \ref nclValidate. Associated data will be stored in  \ref NclEvent::validation. */
	NCL_EVENT_DISCONNECTION,/**< \brief A Nymi's \ref glossaryConnection "connection" has terminated. Associated data will be stored in  \ref NclEvent::disconnection. */
	NCL_EVENT_ECG_START,/**< \brief Finished starting the  \ref glossaryEcg "ECG" stream of a Nymi. Triggered by  \ref nclStartEcgStream. Associated data will be stored in  \ref NclEvent::ecgStart. */
	NCL_EVENT_ECG,/**< \brief A Nymi has sent  \ref glossaryEcg "ECG" data. Triggered by  \ref nclStartEcgStream. Associated data will be stored in  \ref NclEvent::ecg. */
	NCL_EVENT_ECG_STOP,/**< \brief Finished stopping the  \ref glossaryEcg "ECG" of a Nymi. Triggered by  \ref nclStopEcgStream. Associated data will be stored in  \ref NclEvent::ecgStop. */
	NCL_EVENT_VK,/**< \brief A Nymi has sent a verification key. Triggered by  \ref nclCreateSigKeyPair. Associated data will be stored in  \ref NclEvent::vk. */
	NCL_EVENT_SIG,/**< \brief A Nymi has signed a message and sent the signature. Triggered by  \ref nclSign. Associated data will be stored in  \ref NclEvent::sig. */
	NCL_EVENT_GLOBAL_VK,/**< \brief A Nymi has sent a global verification key. Triggered by  \ref nclCreateGlobalSigKeyPair. Associated data will be stored in  \ref NclEvent::globalVk. */
	NCL_EVENT_GLOBAL_SIG,/**< \brief A Nymi has signed a message with a global signing key and sent the signature. Triggered by  \ref nclGlobalSign. Associated data will be stored in  \ref NclEvent::globalSig. */
	NCL_EVENT_CREATED_SK,/**< \brief A Nymi created and sent a symmetric key. Triggered by  \ref nclCreateSk. Associated data will be stored in  \ref NclEvent::createdSk. */
	NCL_EVENT_GOT_SK,/**< \brief A Nymi sent a previously created symmetric key. Triggered by  \ref nclGetSk. Associated data will be stored in  \ref NclEvent::gotSk. */
	NCL_EVENT_PRG,/**< \brief A Nymi sent a pseudorandom value. Triggered by  \ref nclPrg. Associated data will be stored in  \ref NclEvent::prg. */
	NCL_EVENT_RSSI,/**< \brief A Nymi has sent its  \ref glossaryRssi "RSSI". Triggered by  \ref nclGetRssi. Associated data will be stored in  \ref NclEvent::rssi. */
	NCL_EVENT_FIRMWARE_VERSION,/**< \brief A Nymi has sent its firmware version. Triggered by  \ref nclGetFirmwareVersion. Associated data will be stored in  \ref NclEvent::firmwareVersion. */
	NCL_EVENT_NOTIFIED/**< \brief Finished sending a Nymi the notify command. Triggered by  \ref nclNotify. Associated data will be stored in  \ref NclEvent::notified. */
} NclEventType;

/** \brief A list of error codes, each with unique recovery possibilities. Returned by  \ref nclGetErrorCode. \ingroup coreTypes */
typedef enum{
	NCL_ERROR_NULL,/**< \brief No error, or no appropriate programmatic response. */
	NCL_ERROR_NOT_INITED,/**< \brief NCL has not been initialized. Call \ref nclInit. */
	NCL_ERROR_NCL_FAILED,/**< \brief NCL has failed irrecoverably. Do a postmortem if needed and call \ref nclFinish. */
	NCL_ERROR_ECODAEMON_MISSING,/** \brief Could not attach to an ecodaemon. Instruct the user to start an appropriate one. Call \ref nclFinish and then \ref nclInit. */
	NCL_ERROR_SERIAL_FAILED,/**< \brief The Bluegiga dongle serial port has failed. Instruct the user to replug the dongle. Call \ref nclFinish and then \ref nclInit. */
	NCL_ERROR_INVALID_HANDLE,/**< \brief A Nymi handle specified doesn't correspond to an existing Nymi. */
	NCL_ERROR_MISMATCH,/**< \brief The Nymi can't interpret what the NCL sent it or vice versa. Ensure the Nymi and NCL are compatible. */
	NCL_ERROR_WRONG_STATE,/**< \brief A Nymi was in an incorrect state. For example, a provision was requested while validated. */
	NCL_ERROR_BUSY,/**< \brief A Nymi was busy so another procedure couldn't be done. Leave the Nymi alone, wait, and try again. */
	NCL_ERROR_BAD_VALUE,/**< \brief At least one parameter is out of range or doesn't specify a known entity. */
	NCL_ERROR_BAD_PARTNER_KEY,/**< \brief The particular parameter partner key is invalid. Ensure the key is being specified correctly. If it is, check with the key distributor that the key is still valid. If it isn't, get a new one. */
	NCL_ERROR_OUT_OF_MEMORY,/**< \brief The Nymi doesn't have enough memory for the requested action. Ask the user to clear out some memory. */
	NCL_ERROR_LOW_BATTERY,/**< \brief Not enough battery to complete the command. Ask the user to charge the Nymi. */
	NCL_ERROR_NYMI_FAILED/**< \brief Catch-all for other failures on the Nymi that merit a retry. If it continues to fail, the Nymi may be permanently damaged. */
} NclErrorCode;

/** \brief Reasons for disconnection, used in the `.disconnection.reason` field of an  \ref NCL_EVENT_DISCONNECTION  \ref NclEvent. \ingroup negotiationTypes */
typedef enum{
	NCL_DISCONNECTION_LOCAL,/**< \brief Disconnected by us (the  \ref glossaryNea "NEA"). */
	NCL_DISCONNECTION_TIMEOUT,/**< \brief Nymi took too long to do something, so NCL disconnected it. */
	NCL_DISCONNECTION_FAILURE,/**< \brief The Nymi behaved unexpectedly, so NCL disconnected it. */
	NCL_DISCONNECTION_REMOTE,/**< \brief Disconnected by the Nymi. */
	NCL_DISCONNECTION_CONNECTION_TIMEOUT,/**< \brief Disconnected due to no information requests. */
	NCL_DISCONNECTION_LL_RESPONSE_TIMEOUT,/**< \brief Disconnected due to link-layer procedure timeout. */
	NCL_DISCONNECTION_OTHER/**< \brief Disconnected for an unknown reason. */
} NclDisconnectionReason;

/** \brief Which algorithm scheme to use with signatures. See \ref nclCreateSigKeyPair. \ingroup securityTypes */
typedef enum{
	NCL_NIST256P,/**< \brief ECDSA with NISTP256 curve. */
	NCL_SECP256K/**< \brief ECDSA with SECP256K curve. */
} NclSignatureScheme;

/*=====types=====*/
typedef unsigned char NclUInt8;/**< \brief NCL type for unsigned 8-bit integers. \ingroup coreTypes */
typedef short NclSInt16;/**< \brief NCL type for signed 16-bit integers. \ingroup coreTypes */
typedef int NclSInt32;/**< \brief NCL type for signed 32-bit integers. \ingroup coreTypes */

/** \brief NCL type for sending info. See \ref nclInfo. \ingroup coreTypes */
typedef struct{ char string[512]; } NclInfo;

typedef NclUInt8 NclProvisionId[NCL_PROVISION_ID_SIZE];/**< \brief Information required to identify a provision, a part of the  \ref NclProvision struct. \ingroup negotiationTypes */
typedef NclUInt8 NclProvisionKey[NCL_PROVISION_KEY_SIZE];/**< \brief Provision key, a part of the  \ref NclProvision struct. This is a secret key. \ingroup negotiationTypes */

/** \brief Struct to store information about a provision (bridge between an  \ref glossaryNea "NEA" and a Nymi, used for validation the future). \ingroup negotiationTypes */
typedef struct{
	NclProvisionKey key;/**< \brief Secret key, used for secure communication between Nymi and  \ref glossaryNea "NEA". Do not transfer this to other devices and NEAs. */
	NclProvisionId id;/**< \brief The Nymi's name for this  \ref glossaryNea "NEA", it also plays part in deriving the key. The ID doesn't need to be kept secret.  */
	NclBool strong;/**< \brief Whether or not this provision allows strong finds. */
} NclProvision;

typedef NclUInt8 NclPartnerPublicKey[NCL_PARTNER_PUBLIC_KEY_SIZE];/**< \brief Public key issued to partners that allows them to create global signature keys. Paired with an  \ref NclPartnerPrivateKey. \ingroup securityTypes */
typedef NclUInt8 NclPartnerPrivateKey[NCL_PARTNER_PRIVATE_KEY_SIZE];/**< \brief Private key issued to partners that allows them to create global signature keys. Paired with an  \ref NclPartnerPublicKey. \ingroup securityTypes */
typedef NclUInt8 NclVkId[NCL_VK_ID_SIZE];/**< \brief ID of a verification key, produced by  \ref nclCreateSigKeyPair or  \ref nclCreateGlobalSigKeyPair. \ingroup securityTypes */
typedef NclUInt8 NclVk[NCL_VK_SIZE];/**< \brief Public verification key. Part of a signature key pair. Used to verify a Nymi's signature of some message. Produced by  \ref nclCreateSigKeyPair or  \ref nclCreateGlobalSigKeyPair. \ingroup securityTypes */
typedef NclUInt8 NclMessage[NCL_MESSAGE_SIZE];/**< \brief Signable message, supplied to  \ref nclSign or  \ref nclGlobalSign. \ingroup securityTypes */
typedef NclUInt8 NclSig[NCL_SIG_SIZE];/**< \brief A signature, produced by the Nymi after  \ref nclSign or  \ref nclGlobalSign is called. \ingroup securityTypes */
typedef NclUInt8 NclAdv[NCL_ADV_SIZE];/**< \brief A Nymi's advertisement. Used with \ref nclGlobalSign. \ingroup securityTypes */

typedef NclUInt8 NclSkId[NCL_SK_ID_SIZE];/**< \brief ID of a symmetric key, produced by  \ref nclCreateSk. \ingroup securityTypes */
typedef NclUInt8 NclSk[NCL_SK_SIZE];/**< \brief A symmetric key stored on the Nymi, produced by  \ref nclCreateSk. This is a secret, so zero any copy you make (with  \ref nclCreateSk or  \ref nclGetSk) after use. \ingroup securityTypes */
typedef NclUInt8 NclPrg[NCL_PRG_SIZE];/**< \brief A pseudorandom value, produced by  \ref nclPrg. \ingroup securityTypes */

typedef NclUInt8 NclFirmwareVersion[NCL_FIRMWARE_VERSION_SIZE];/**< \brief Firmware version, returned by  \ref nclGetFirmwareVersion. \ingroup miscTypes */

/*=====events=====*/
/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_INIT, the NclEventInit `event.init` can be accessed.
\ingroup coreTypes
**/
typedef struct{
	NclBool success;/**< \brief Whether or not the operation finished successfully. */
} NclEventInit;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_ERROR, the NclEventError `event.error` can be accessed.
\ingroup coreTypes
**/
typedef struct{
	NclErrorCode code;/**< \brief The code of the error that occurred. */
} NclEventError;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_DISCOVERY, the NclEventDiscovery `event.discovery` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that was \ref glossaryDiscovery "discovered". */
	int rssi;/**< \brief The  \ref glossaryRssi "RSSI" value in dB. */
} NclEventDiscovery;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_FIND, the NclEventFind `event.find` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that was \ref glossaryFinding "found". */
	int rssi;/**< \brief The  \ref glossaryRssi "RSSI" value in dB. */
	NclProvisionId provisionId;/**< \brief The ID of the  \ref glossaryProvisioning "provision" with which this Nymi was found. */
	NclBool strong;/**< \brief If a find is strong, you can be sure that it came from the Nymi you provisioned with. Otherwise you have to validate to be sure. */
} NclEventFind;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_DETECTION, the NclEventDetection `event.detection` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that was \ref glossaryDetection "detected". */
	int rssi;/**< \brief The RSSI value in dB. */
} NclEventDetection;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`: 
- If `event.type` is  \ref NCL_EVENT_AGREEMENT, the NclEventAgreement `event.agreement` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that was agreed with. */
	NclBool leds[NCL_AGREEMENT_PATTERNS][NCL_LEDS];/**< \brief Whether each LED on the Nymi is lit for each pattern. The (0, 0) entry is the earliest state of the leftmost LED. */
} NclEventAgreement;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_PROVISION, the NclEventProvision `event.provision` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that was  \ref provision "provisioned". */
	NclProvision provision;/**< \brief The new provision. */
} NclEventProvision;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_VALIDATION, the NclEventCompletion `event.validation` can be accessed.
- If `event.type` is  \ref NCL_EVENT_ECG_START, the NclEventCompletion `event.ecgStart` can be accessed.
- If `event.type` is  \ref NCL_EVENT_ECG_STOP, the NclEventCompletion `event.ecgStop` can be accessed.
- If `event.type` is  \ref NCL_EVENT_NOTIFIED, the NclEventCompletion `event.notified` can be accessed.
\ingroup negotiationTypes streamTypes miscTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that completed a procedure. */
} NclEventCompletion;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_DISCONNECTION, the NclEventDisconnection `event.disconnection` can be accessed.
\ingroup negotiationTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that just disconnected. See \ref glossaryConnection "connection". */
	NclDisconnectionReason reason;/**< \brief The reason for disconnection. Note that different platforms discern different sets of reasons, so you should think of this field as "bonus information, if available". */
} NclEventDisconnection;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_ECG, the NclEventEcg `event.ecg` can be accessed.
\ingroup streamTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending \ref glossaryEcg "ECG" samples. */
	NclSInt32 samples[NCL_ECG_SAMPLES_PER_EVENT];/**< \brief The ECG samples, each is a 3-byte signed integer. The 0th sample is the oldest. */
} NclEventEcg;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_VK, the NclEventVk `event.vk` can be accessed.
- If `event.type` is  \ref NCL_EVENT_GLOBAL_VK, the NclEventVk `event.globalVk` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The handle of the Nymi that sent a verification key. */
	NclVkId id;/**< \brief ID of the verification key. */
	NclVk vk;/**< \brief The verification key. */
} NclEventVk;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`: 
- If `event.type` is  \ref NCL_EVENT_SIG, the NclEventSig `event.sig` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending a signature. */
	NclSig sig;/**< \brief The signature of the previously specified message. */
} NclEventSig;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`: 
- If `event.type` is  \ref NCL_EVENT_GLOBAL_SIG, the NclEventSig `event.globalSig` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending a signature. */
	NclSig sig;/**< \brief The signature of the previously specified message. */
	/** \brief
	The ID of the verification key that can verify this signature.
	This is needed here because the signature requester doesn't know which Nymi it's connected to, and so it wouldn't know which verification key to use.
	**/
	NclVkId vkId;
} NclEventGlobalSig;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_CREATED_SK, the NclEventSk `event.createdSk` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending a symmetric key. */
	NclSkId id;/**< \brief ID of the symmetric key. */
	NclSk sk;/**< \brief The symmetric key. This is secret, so zero it and any copy after use!  */
} NclEventCreatedSk;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_GOT_SK, the NclEventSk `event.gotSk` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending a symmetric key. */
	NclSk sk;/**< \brief The symmetric key. This is secret, so zero it and any copy after use! */
} NclEventGotSk;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_PRG, the NclEventPrg `event.prg` can be accessed.
\ingroup securityTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that sent a pseudorandom value. */
	NclPrg value;/**< \brief The pseudorandom value. */
} NclEventPrg;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`:
- If `event.type` is  \ref NCL_EVENT_RSSI, the NclEventRssi `event.rssi` can be accessed.
\ingroup miscTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that is sending  \ref glossaryRssi "RSSI". */
	int rssi;/**< \brief The RSSI value in dB. */
} NclEventRssi;

/** \details A type of data-containing member of NclEvent. Say we received an  \ref NclEvent called `event`: 
- If `event.type` is  \ref NCL_EVENT_FIRMWARE_VERSION, the NclEventFirmwareVersion `event.firmwareVersion` can be accessed.
\ingroup miscTypes
**/
typedef struct{
	int nymiHandle;/**< \brief The \ref glossaryNymiHandle "handle" of the  \ref glossaryNymi "Nymi" that sent its firmware version. */
	NclFirmwareVersion version;/**< \brief The firmware version currently installed on the Nymi. */
} NclEventFirmwareVersion;

/** \brief The  \ref glossaryNcl "NCL" is event-driven. NclEvent is a struct that represents an event triggered by an NCL operation. Each instance of NclEvent has two members: one that defines the type of event, and one that carries the associated data.  \ingroup coreTypes */
typedef struct{

	/** @name Event Type
	 *  All instances of NclEvent has a member with the name "type". For example, `%NclEvent event` can be queried using `event.type` to determine what type of event it represents. `event.type` is an enum of the type %NclEventType. 
	 Clicking on  \ref NclEventType will show you all its possible values. 
	 */
	/**@{*/
	NclEventType type;/**< \brief The type of the event that was triggered. */
	/**@}*/

	/*Hiding this anonymous union because it confuses Doxygen.*/
	/** \cond NEVER */
	union{
	/** \endcond */


	/** @name Event Data
	 * The data-containing member of NclEvent is implemented as an anonymous union. This means that the same chunk of memory has a different name and stores different types of data depending on the type of event.
	Since different types of events would have different types of associated data, this a memory-efficient implementation. 

	For example, if `event.type` is NCL_EVENT_INIT, then its data is stored in `event.init`. If `event.type` is NCL_EVENT_DISCOVERY, then its data is stored in `event.discovery`.
	
	`event.discovery` is a struct of the type %NclEventDiscovery. 
	Clicking on  \ref NclEventDiscovery will show you its members. Now you know that `event.discovery.rssi` tells you the RSSI value of the Nymi that was discovered. 
	 */
	/**@{*/
		NclEventInit init;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_INIT. */
		NclEventError error;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_ERROR. */
		NclEventDiscovery discovery;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_DISCOVERY. */
		NclEventFind find;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_FIND. */
		NclEventDetection detection;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_DETECTION. */
		NclEventAgreement agreement;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_AGREEMENT. */
		NclEventProvision provision;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_PROVISION. */
		NclEventCompletion validation;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_VALIDATION. */
		NclEventDisconnection disconnection;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_DISCONNECTION. */
		NclEventCompletion ecgStart;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_ECG_START. */
		NclEventEcg ecg;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_ECG. */
		NclEventCompletion ecgStop;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_ECG_STOP. */
		NclEventVk vk;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_VK. */
		NclEventSig sig;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_SIG. */
		NclEventVk globalVk;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_GLOBAL_VK. */
		NclEventGlobalSig globalSig;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_GLOBAL_SIG. */
		NclEventCreatedSk createdSk;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_CREATED_SK. */
		NclEventGotSk gotSk;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_GOT_SK. */
		NclEventPrg prg;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_PRG. */
		NclEventCompletion notified;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_NOTIFIED. */
		NclEventRssi rssi;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_RSSI. */
		NclEventFirmwareVersion firmwareVersion;/**< \brief Data-containing member for an %NclEvent of type  \ref NCL_EVENT_FIRMWARE_VERSION. */
	/**@}*/
	
	/*Hiding this anonymous union because it confuses Doxygen.*/
	/** \cond NEVER */
	};
	/** \endcond */
	
} NclEvent;

/*=====callbacks=====*/
/** \brief Function pointer type for callbacks, used in the parameter list of  \ref nclInit and  \ref nclAddBehavior. \ingroup coreTypes */
typedef void (*NclCallback)(NclEvent event, void* userData);

/*=====functions=====*/
/*! \brief Initialize the library and start  \ref glossaryBle "BLE" initialization.
\ingroup coreFunctions

\details
\copybrief nclInit
If this function fails, there is no guarantee of recoverability.
This is because failure will be caused by something beyond the scope of the program, for example lack of appropriate hardware.

\pre nclInit has never been called, or nclFinish has just been called.
\post On successful initiation, an \ref NCL_EVENT_INIT will be triggered. Until \ref nclFinish is called, \ref NCL_EVENT_ERROR will be triggered if an error occurs.

\param [in] callback Callback function that at least handles NCL_EVENT_INIT. This callback is added as if `nclAddBehavior(callback, userData, NCL_EVENT_ANY, NCL_NYMI_HANDLE_ANY)` were called.
\param [in] userData Any custom data you would like to pass into the callback function when it is triggered. You can pass NULL if you don't have any custom data.
\param [in] name The name of your NEA. (A user will be able to see a list of connected apps from the Nymi Companion App). 18 characters or less, not including null terminator.
\param [in] mode A bit mask representing the desired mode. See \ref NclMode.
\param [in] errorStream Error messages are sent here. Typically this would be set to stderr, or a file stream. Pass NULL if you don't want errors printed. You should habitually check the error stream if anything goes wrong.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclInit(
	NclCallback callback,
	void* userData,
	const char* name,
	NclMode mode,
	FILE* errorStream
);

/*! \brief Terminate the library. 
\ingroup coreFunctions

\details
\copybrief nclFinish
If this function fails, there is no guarantee of recoverability, so a program should exit.
This is because failure will be caused by something beyond the scope of the program.
Do not call this function from within an NCL callback.

\pre NCL is initialized.
\post All resources NCL took are released and \ref nclInit may be called again.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclFinish();

/*! \brief Get info about the current state of NCL.
\ingroup coreFunctions

\details
\copybrief nclInfo
If NCL as a whole isn't acting like you'd expect, call this function.
The following information will be returned, and you should print it:
- NCL version
- NCL flavor
- The current state of NCL
- What should be expected given the current state of NCL
Additionally, the error stream will be tested if appropriate by writing an error that says this function was called.
It's important to understand all of the results.
Knowing the result of this function will help communicate with others exactly what is going on.

\return Info about the current state of NCL.
*/
NCL NclInfo nclInfo();

/*! \brief In synchronous mode, send callbacks.
\ingroup coreFunctions

\details
In synchronous mode (set by  \ref nclInit), this function must be called at regular intervals to send accumulated events to callbacks.
This function returns  \ref NCL_FALSE in asynchronous mode.

\pre NCL is in synchronous mode.
\post Queued events will have triggered appropriate callbacks.

\param [in] timeout Maximum time in milliseconds to wait for events.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclUpdate(unsigned timeout);

/*! \brief Set the IP and port of the ecodaemon, if applicable.
\ingroup coreFunctions

\details
Some flavors of NCL use an ecodaemon to talk to the Nymi ecosystem.
The ecodaemon is communicated with over the network.
There is a default IP address and port number at which to find the ecodaemon, so in most cases you don't need to worry about this function.
However, if you are using an ecodaemon, and if you need it to be at a custom IP address or port, you can call this function to tell the \ref glossaryNea "NEA" where to find the ecodaemon.

\pre You are using an ecodaemon NCL flavor. NCL is not initialized.
\post The IP address and port to find the ecodaemon during \ref nclInit will be set.

\param [in] ip Null-terminated string representation of the IP address. By default, this is "127.0.0.1".
\param [in] port The port number. By default, this is 9089.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclSetIpAndPort(const char* ip, int port);

/*! \brief Add a callback to be called under certain circumstances.
\ingroup coreFunctions

\details
A behavior consists of: the callback function to use, custom data for the callback, the event type that should trigger the callback, and the handle of the Nymi to listen to.
When an event is triggered, the NCL checks an internal list of behaviors and calls all callbacks whose event type and Nymi handle match the event, in the order they were added.
This function allows you to add behaviors.
You cannot add the same behavior twice (i.e. same callback function, user data, event type, and Nymi handle).

\pre NCL is initialized.
\post On success, the specified behavior is added.

\param [in] callback A callback function.
\param [in] userData Any custom data you would like to pass into the callback function when it is triggered. Pass in NULL if you don't have any custom data.
\param [in] eventType The type of event you'd like the callback to receive. Pass in  \ref NCL_EVENT_ANY to listen for all events.
\param [in] nymiHandle The integer handle of the Nymi you'd like the callback to respond to. Pass in  \ref NCL_NYMI_HANDLE_ANY to listen to all Nymis and events without Nymi handles.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclAddBehavior(NclCallback callback, void* userData, NclEventType eventType, int nymiHandle);

/*! \brief Remove a behavior added with \ref nclAddBehavior.
\ingroup coreFunctions

\details
This function allows you to remove behaviors that were added using  \ref nclAddBehavior or  \ref nclInit, by specifying their parameters when added.
Note that this function removes one behavior at a time, so you cannot remove multiple behaviors with  \ref NCL_EVENT_ANY or  \ref NCL_NYMI_HANDLE_ANY.

\pre NCL is initialized.
\post On success, the specified behavior is removed.

\param [in] callback Callback of the previously specified behavior to remove.
\param [in] userData User data of the previously specified behavior to remove.
\param [in] eventType Event type of the previously specified behavior to remove.
\param [in] nymiHandle Nymi handle of the previously specified behavior to remove.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclRemoveBehavior(NclCallback callback, void* userData, NclEventType eventType, int nymiHandle);

/*! \brief Lock the error stream.
\ingroup coreFunctions

\details
In asynchronous mode (set by  \ref nclInit), the error stream may be written to at any point.
Say you provided to nclInit a stream that you also want to be able to write to.
If you try to write to the stream without locking it, the NCL could interrupt your write with its own write, creating a gibberish stream.
This function allows you to lock it (i.e. prevent additional errors from being written).
Make sure you unlock the error stream with  \ref nclUnlockErrorStream after you are done with it.
This function returns  \ref NCL_FALSE in synchronous mode.

\pre NCL is in asynchronous mode.
\post The NCL error stream is locked.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclLockErrorStream();

/*! \brief Unlock the error stream.
\ingroup coreFunctions

\details 
After calling  \ref nclLockErrorStream, this function allows you to unlock the error stream so that more errors can be written to it. 

\pre NCL is in asynchronous mode.
\post The NCL error stream is unlocked.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclUnlockErrorStream();

/*! \brief Check for more information on a failure in hopes of responding programmatically.
\ingroup coreFunctions

\details
Error codes are meant to give enough information to differentiate between which possible programmatic responses to failures should happen.
When a failure happens, if the error code is currently clear, the error code is set.
This function returns the current error code and then clears it.
A failure is typically indicated by an NCL function returning \ref NCL_FALSE, or an \ref NCL_EVENT_ERROR;
This function gives the same concurrency guarantees that other NCL functions do, but you still need to analyze your own concurrency design to ensure that the returned error code is used properly.

\pre NCL is initialized.
\post The NCL error code is cleared.

\return The error code (See the  \ref NclErrorCode enum).
*/
NCL NclErrorCode nclGetErrorCode();

/*! \brief Start scanning for provisioning  \ref glossaryNymi "Nymis".
\ingroup negotiationFunctions

\copybrief nclStartDiscovery

\pre NCL is initialized.
\post A \ref glossaryScanning "scan" is started. When a Nymi is \ref glossaryDiscovery "discovered", an \ref NCL_EVENT_DISCOVERY will be triggered.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclStartDiscovery();

/*! \brief Start scanning for previously provisioned Nymis.
\ingroup negotiationFunctions

\copybrief nclStartFinding

\pre NCL is initialized.
\post A \ref glossaryScanning "scan" is started. When a Nymi is \ref glossaryFinding "found", an \ref NCL_EVENT_FIND will be triggered. If enabled, when a Nymi is \ref glossaryDetection "detected", an \ref NCL_EVENT_DETECTION is triggered.

\param [in] provisions Array of provisions, one for each Nymi you want to find.
\param [in] numberOfProvisions Number of provisions in the provisions array.
\param [in] detect Whether or not to trigger detections.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclStartFinding(
	const NclProvision* provisions,
	unsigned numberOfProvisions,
	NclBool detect
);

/*! \brief Stop scanning for Nymis
\ingroup negotiationFunctions

\details
Stops  \ref glossaryScanning "scanning". This is useful for setting a time-out for when the  \ref glossaryNea "NEA" does not find any Nymis.

\pre NCL is initialized.
\post Scanning is stopped.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclStopScan();

/*! \brief Clear the internal list scanned of  \ref glossaryNymi "Nymis"
\ingroup negotiationFunctions

\details
The NCL distinguishes Nymis based on their MAC address, this allows a Nymi that has been scanned twice to be labeled with the same  \ref glossaryNymiHandle "handle".
However, Nymis, when not  \ref glossaryConnection "connected", switch their MAC addresses periodically so that they can't be tracked.
When this happens, the corresponding Nymi handle will no longer work but the information about the Nymi is still stored by the NCL.  
Since a large internal list of Nymis may slow down performance or eventually cause a crash, you may want to clear the internal list of the Nymis.
All Nymi handles assigned before a call to this function become invalid when it is called.
For this reason, you may only call this function when no Nymi is connected.

\pre NCL is initialized and no Nymi is connected.
\post The internal list of Nymis is cleared.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclClearScannedNymis();

/*! \brief Suggest BLE connection parameters. Setting them may not be possible, depending on your system. 
\ingroup negotiationFunctions

\details
This function will affect the next \ref glossaryConnection "connection" you make.
With Core Bluetooth (iOS and Mac), you do not have control over these parameters. See the Bluetooth Accessory Design Guidelines for Apple products.
If the vocabulary used here is unfamiliar, you may want to look at a  \ref glossaryBle "BLE" resource.

\pre NCL is initialized.
\post The next connection will use the specified parameters.

\param [in] intervalMin Minimum connection interval. Range is 7.5ms to 4s. Where applicable, default is 10ms. In units of 1.25ms.
\param [in] intervalMax Maximum connection interval. Must be greater than intervalMin. Range is 7.5ms to 4s. Where applicable, default is 625ms. In units of 1.25ms.
\param [in] timeout Supervision timeout. Must be greater than connection interval. Range: 100ms to 32s. Where applicable, default is 3s. In units of 10ms.
\param [in] latency Slave latency. Range is 0 to 500. Where applicable, default is 10.

\return  \ref NCL_TRUE Parameters accepted.
\return  \ref NCL_FALSE One or more parameters not guaranteed to be as specified.
*/
NCL NclBool nclHintConnectionParams(
	unsigned intervalMin,
	unsigned intervalMax,
	unsigned timeout,
	unsigned latency
);

/*! \brief Connect to a provisioning Nymi that's been discovered, and derive an LED pattern.
\ingroup negotiationFunctions

\details
When  \ref glossaryNymi "Nymis" are provisioning, they are indistinguishable, which is important in ensuring they remain untrackable.
However, it also makes it harder for a user to be sure the correct Nymi and  \ref glossaryNea "NEA" have found each other when provisioning.
This is what agreement solves.
An LED pattern is derived and the Nymi will display it.
You are responsible for making your  \ref glossaryNea "NEA" display it as well.
The user will check to make sure both devices are displaying the same pattern, then allow the provisioning process to complete.

\pre NCL is initialized. The specified Nymi has been recently discovered and is not connected.
\post A \ref glossaryConnection "connection" with the specified Nymi is established, its \ref glossaryCommandChannel "command channel" is taken, and \ref NCL_EVENT_AGREEMENT will be triggered if it agrees, at which point its command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to agree with.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclAgree(int nymiHandle);

/*! \brief Provision a Nymi that has been agreed with.
\ingroup negotiationFunctions

\details
Request to \ref glossaryProvisioning "provision" a \ref glossaryNymi "Nymi".
An  \ref NCL_EVENT_PROVISION  \ref NclEvent is triggered if provisioning succeeds.
The provision event will contain an  \ref NclProvision.
You should store this provision locally.
Erasing it later is sufficient for effectively revoking a Nymi.

\pre NCL is initialized. The specified Nymi has \ref glossaryAgreement "agreed", has not disconnected since, and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken, and an \ref NCL_EVENT_PROVISION will be triggered if the specified Nymi is provisioned, at which point its command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to create a provision for.
\param [in] strong \ref NCL_TRUE if you want to enable strong finding for this provision, NCL_FALSE if not.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclProvision(int nymiHandle, NclBool strong);

/*! \brief Validate a Nymi.
\ingroup negotiationFunctions

\details
\copybrief nclValidate

\pre NCL is initialized. The specified Nymi has been \ref glossaryFinding "found" recently and is not connected.
\post A \ref glossaryConnection "connection" with the specified Nymi is established, the Nymi's \ref glossaryCommandChannel "command channel" is taken, and \ref NCL_EVENT_VALIDATION will be triggered if it is validated, at which point the Nymi's command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to validate.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclValidate(int nymiHandle);

/*! \brief Get which Nymis are connected.
\ingroup negotiationFunctions

\details
This function provides the \ref glossaryNymiHandle "handles" of all the \ref glossaryNymi "Nymis" that are currently  \ref glossaryConnection "connected".

\pre NCL is initialized.

\param [out] nymiHandles An array in which to put connected Nymi handles. Unused elements will be set to \ref NCL_NYMI_HANDLE_ANY.
\param [in,out] nymiHandlesSize Upon calling, a pointer to a variable containing the maximum number of Nymi handles that can be stored in the nymiHandles array. After returning, the variable pointed to will be the number of Nymi handles written into the Nymi handle array.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclGetConnected(int* nymiHandles, unsigned* nymiHandlesSize);

/*! \brief Disconnect from a Nymi.
\ingroup negotiationFunctions

\details
Disconnect a \ref glossaryNymi "Nymi".
It is a good idea to call this function on all connected Nymis prior to calling  \ref nclFinish.
You can use  \ref nclGetConnected to find out which Nymis are connected.

\pre NCL is initialized and the specified Nymi is \ref glossaryConnection "connected".
\post The specified Nymi will be disconnected.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to disconnect.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclDisconnect(int nymiHandle);

/*! \brief Send a simple notification to the Nymi wearer.
\ingroup miscFunctions

\details
The LED lights and the vibration motor of the \ref glossaryNymi "Nymi" are mainly used for built-in functions such as reporting the state of the Nymi.
As a developer, you can send simple notifications to the user to report either a "good" state or a "bad" state. 
A "good" notification makes the Nymi vibrate and light up all its LEDs.
A "bad" notification makes the Nymi vibrate and blink all its LEDs.

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_NOTIFIED will be triggered when the command channel is free. The Nymi will notify its user.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi whose user you want to notify.
\param [in] good  \ref NCL_TRUE to send a "good" notification,  \ref NCL_FALSE to send a "bad" notification.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclNotify(int nymiHandle, NclBool good);

/*! \brief Start the ECG stream of a Nymi.
\ingroup streamFunctions

\details
Starts a stream of ECG samples, with a rate of 250Hz. The user should put a finger from the opposing hand on the top metal piece of the Nymi to measure their ECG.

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_ECG_START will be triggered when the command channel is free, and then \ref NCL_EVENT_ECG events will be triggered as the Nymi streams ECG.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi whose ECG stream you want to start.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclStartEcgStream(int nymiHandle);

/*! \brief Stop the ECG stream of a Nymi.
\ingroup streamFunctions

\details
Stops the stream of ECG samples. 

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_ECG_STOP will be triggered when the command channel is free. At this point no more \ref NCL_EVENT_ECG events will be triggered.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi whose ECG stream you want to stop.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclStopEcgStream(int nymiHandle);

/*! \brief Tell a Nymi to create an NEA-specific pair of cryptographic keys for use with signatures.
\ingroup securityFunctions

\details
This function tells a Nymi to create a pair of cryptographic keys for use with signatures:
a public verification key that is sent in an  \ref NCL_EVENT_VK  \ref NclEvent,
and a private signing key that is stored on the Nymi and kept secret.
Only the  \ref glossaryNea "NEA" that called this function can later ask for signatures using  \ref nclSign. 
The signature can then be verified using the verification key.

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_VK will be triggered when the key pair has been created, at which point the command channel is free.

\param [in] signatureScheme The signature scheme to use. Signature created with this key pair must be verified with the same scheme.
\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to create a signature key pair for.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclCreateSigKeyPair(int nymiHandle, NclSignatureScheme signatureScheme);

/*! \brief Tell a Nymi to sign a message with a private signing key.
\ingroup securityFunctions

\details
\copybrief nclSign
The message can be verified with the verification key that corresponds to the supplied verification key ID.

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel". The signature key pair corresponding to the specified VK ID has been created on the specified Nymi (see \ref nclCreateSigKeyPair).
\post The Nymi's command channel is taken. An \ref NCL_EVENT_SIG will be triggered when the Nymi signs the message, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to sign a message.
\param [in] vkId ID of a verification key that will be able to verify the signature.
\param [in] message The message to sign. An array of bytes.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclSign(
	int nymiHandle,
	const NclVkId vkId,
	const NclMessage message
);

/*! \brief Tell a Nymi to create a global pair of cryptographic keys for use with signatures.
\ingroup securityFunctions

\details
This function creates global signature key pair:
a public verification key that is sent to the  \ref glossaryNea "NEA" in an  \ref NCL_EVENT_GLOBAL_VK  \ref NclEvent,
and a private signing key that is stored on the Nymi and kept secret.
To use this function, you must have a partner key pair that was issued by Bionym Inc., the maker of the Nymi. 
The created global signature key pair is associated with the supplied partner key pair.
A Nymi will associate only one global signature key pair to a partner key pair at a time.

Unlike  \ref nclCreateSigKeyPair, this function creates a _global_ signature key pair. 
Global signature key pairs are unique in that any  \ref glossaryNea "NEA" with the partner key pair can later ask the Nymi for a signature (via  \ref nclGlobalSign), not just the  \ref glossaryNea "NEA" that created the key pair.
The signature can then be verified using the verification key. This verification key may be sent over a network or stored in a cloud service. 

Potential use cases for global signature key pairs include access control within a building, payment terminals, and any scenario where cross-device functionality is required. 

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_GLOBAL_VK will be triggered when the key pair has been created, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to create a global signature key pair.
\param [in] partnerPublicKey The partner public key of the partner key pair to associate with this global signature key pair.
\param [in] bionymSignature Bionym-issued signature of the partner public key.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclCreateGlobalSigKeyPair(int nymiHandle, const NclPartnerPublicKey partnerPublicKey, const NclSig bionymSignature);

/*! \brief Get the advertisement of a detected Nymi.
\ingroup securityFunctions

\details
This is a step in making a global signature.
After \ref glossaryFinding "finding" or \ref glossaryDetection "detecting" a Nymi,
use this function to get an advertisement.
The next step is to sign the advertisment with a partner private key.
See \ref nclSignAdv.

\pre NCL is initialized. The specified Nymi has been \ref glossaryDetection "detected" or \ref glossaryFinding "found".

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi whose advertisement you want.
\param [out] adv The specified Nymi's advertisement.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclGetAdv(int nymiHandle, NclAdv adv);

/*! \brief Sign an advertisement with a partner private key.
\ingroup securityFunctions

\details
This is a step in making a global signature.
You may want to implement equivalent functionality yourself elsewhere.
After getting an advertisement with \ref nclGetAdv,
use this function to sign an advertisement.
The next step is to call \ref nclGlobalSign on the Nymi you got the advertisement from.

\pre NCL is initialized.

\param [in] adv The advertisement to sign.
\param [in] message The message that will be signed by the Nymi.
\param [in] partnerPrivateKey The parter private key to use to sign.
\param [out] advSig The signature fo the advertisement.

\return  \ref NCL_TRUE On success.
\return  \ref NCL_FALSE On failure.
*/
NCL NclBool nclSignAdv(const NclAdv adv, const NclMessage message, const NclPartnerPrivateKey partnerPrivateKey, NclSig advSig);

/*! \brief Tell a Nymi to sign a message with a global signature private key.
\ingroup securityFunctions

\details
This is the final step in making a global signature.
After signing an advertisement (see \ref nclSignAdv),
use this function to make a global signature.
The signature of the Nymi's advertisement must have been made with the partner private key that corresponds with the partner public key.
The signature that will be made will be verifiable by the verification key of the signature key pair associated with the partner key pair.
Global signature key pairs always use the \ref NCL_NIST256P scheme.

\pre NCL is initialized. The supplied advertisement has been recently gotten from the specified Nymi. A global signature key pair associated with the specified partner public key has been created on the specified Nymi (see \ref nclCreateGlobalSigKeyPair).
\post A connection is established with the Nymi, and its \ref glossaryCommandChannel "command channel" is taken. An \ref NCL_EVENT_GLOBAL_SIG will be triggered when the Nymi signs the message, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to sign a message.
\param [in] advSig The signature of the specified Nymi's advertisement.
\param [in] partnerPublicKey Partner public key of the partner key pair associated with the signature key pair whose verification key should be able to verify the signature.
\param [in] message The message to sign.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclGlobalSign(
	int nymiHandle,
	const NclSig advSig,
	const NclPartnerPublicKey partnerPublicKey,
	const NclMessage message
);

/*! \brief Verify the signature of a message.
\ingroup securityFunctions

\details
This function verifies a signature created by a call to  \ref nclSign or  \ref nclGlobalSign.
If this signature was created with  \ref nclSign, then the verification key is that whose ID was sent to \ref nclSign, and the signature scheme needs to be whatever you supplied to \ref nclCreateSigKeyPair.
If this signature was created with  \ref nclGlobalSign, then the verification key is that of the signature key pair associated with the partner key pair supplied to  \ref nclGlobalSign, and the signature scheme need to be \ref NCL_NIST256P.

\pre NCL is initialized.

\param [in] vk The verification key associated with the signature.
\param [in] message The message that has been signed.
\param [in] sig The signature that the Nymi supplied.
\param [in] signatureScheme The scheme to use to verify.

\return  \ref NCL_TRUE If the signature has been verified.
\return  \ref NCL_FALSE If the signature has failed verification.
*/
NCL NclBool nclVerify(
	const NclVk vk,
	const NclMessage message,
	const NclSig sig,
	NclSignatureScheme signatureScheme
);

/*! \brief Tell a Nymi to create and send a new symmetric key.
\ingroup securityFunctions

\details
This function prompts the Nymi to create a symmetric key, which can then be used for symmetric encryption. 
The ID of the key is in the `.createdSk.id`  field of the NclEvent struct, this is used to retrieve the key later.
Be careful how you handle the symmetric key!

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_CREATED_SK will be triggered when the Nymi creates the key, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to create a symmetric key.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclCreateSk(int nymiHandle);

/*! \brief Retrieve a previously created symmetric key from a Nymi.
\ingroup securityFunctions

\details
This function retrieves the symmetric key associated with a specified ID, that was created using  \ref nclCreateSk.
Be careful how you handle the symmetric key!

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_GOT_SK will be triggered when the Nymi sends the key, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to get a previously created symmetric key from.
\param [in] id The ID of the symmetric key you want to retrieve.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclGetSk(int nymiHandle, const NclSkId id);

/*! \brief Generate a pseudorandom value from a Nymi.
\ingroup securityFunctions

\details
This function asks the Nymi to generate a pseudorandom value. 

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_PRG will be triggered when the Nymi creates the value, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to get a pseudorandom value from.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclPrg(int nymiHandle);

/*! \brief Get the RSSI of a Nymi.
\ingroup miscFunctions

\details
\ref glossaryRssi "RSSI" can be used to estimate the distance between the Nymi and the device that the Nymi-Enabled  \ref glossaryNea "NEA" is running on. Smaller RSSI indicates larger distance.
Due to noise in RSSI values, you may wish to collect 5-15 samples in sequence and use the mean or median value.
This function is not guaranteed to produce a corresponding event, and there are a few reasons why.
RSSI is present in every BLE packet, this operation simply captures the RSSI on the next packet and sends it along.
As such, this operation may happen while the \ref glossaryCommandChannel "command channel" is taken.
This also means that a failure to trigger an RSSI event means greater failures are happening.
You may also call this function more than once before an RSSI event is triggered.

\pre NCL is initialized. The specified Nymi is \ref glossaryConnection "connected".
\post An \ref NCL_EVENT_RSSI may be triggered.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi you want to get RSSI from.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclGetRssi(int nymiHandle);

/*! \brief Get the firmware version of a Nymi.
\ingroup miscFunctions

\details
Asks the Nymi to send its firmware version.

\pre NCL is initialized. The specified Nymi is \ref glossaryValidation "validated" and has a free \ref glossaryCommandChannel "command channel".
\post The Nymi's command channel is taken. An \ref NCL_EVENT_FIRMWARE_VERSION will be triggered, at which point the command channel is free.

\param [in] nymiHandle The \ref glossaryNymiHandle "handle" of the Nymi whose firmware version you want.

\return  \ref NCL_TRUE On successful initiation.
\return  \ref NCL_FALSE On failed initiation.
*/
NCL NclBool nclGetFirmwareVersion(int nymiHandle);

#ifdef __cplusplus
}
#endif

#endif
