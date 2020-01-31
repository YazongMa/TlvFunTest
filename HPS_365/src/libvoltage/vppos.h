/* Copyright 2009-2015, Voltage Security, all rights reserved. */

#ifndef __VP_POS_H__
#define __VP_POS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "Targets/TargetConfig.h"

/** VpPosContext is an opaque handle to the POS context. */
typedef struct VpPosContextDef* VpPosContext;

/** VpPosKey is an opaque handle to a key. */
typedef struct VpPosKeyDef* VpPosKey;

/** RSECRET and TID MUST be exactly 16 bytes in length. */
#define VP_POS_RSECRET_SIZE     16
#define VP_POS_TID_SIZE         16

/** Pass the VP_POS_DATA_TYPE_PAN value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted is PAN data (i.e. 12-50 ASCII digits).
 */
#define VP_POS_DATA_TYPE_PAN      1

/** Pass the VP_POS_DATA_TYPE_MID value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted is MID data. MID data must be 4 to 23 ASCII digits (e.g 0-9).
 */
#define VP_POS_DATA_TYPE_MID      2

/** Pass the VP_POS_DATA_TYPE_TRACK_1 value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted is track 1 data (i.e. up to 79 ASCII characters
 * corresponding to the ISO 7813 64-character alphabet). The domain of valid
 * Track 1 data depends on the version of the POS, see 
 * VP_POS_FLAGS_ENCRYPT_SPE for details.
 */
#define VP_POS_DATA_TYPE_TRACK_1  3

/** Pass the VP_POS_DATA_TYPE_TRACK_2 value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted is track 2 data (i.e. up to 40 ASCII characters
 * corresponding to the ANSI/ISO BCD 16-character alphabet).
 * The domain of valid Track 2 data depends on the version of the POS, see 
 * VP_POS_FLAGS_ENCRYPT_SPE for details.
 */
#define VP_POS_DATA_TYPE_TRACK_2  4

/** Pass the VP_POS_DATA_TYPE_TRACK_3 value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted is track 3 data (i.e. up to 107 ASCII characters
 * corresponding to the ANSI/ISO BCD 16-character alphabet).
 * The data must start with the start sentinel value, ';'.
 */
#define VP_POS_DATA_TYPE_TRACK_3  5

/** Pass the VP_POS_DATA_TYPE_PAN_AND_CVV value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being
 * encrypted contains both a PAN and the corresponding CVV2 value, separated
 * by a well-known delimiter (CVV2_DELIMITER_CHAR = ':')
 */
#define VP_POS_DATA_TYPE_PAN_AND_CVV  8

/** Pass the VP_POS_DATA_TYPE_ALPHANUMERIC_STRING value as the data type argument
 * to the VpPosEncryptCardData call to indicate that the data being encrypted is
 * to be interpreted as an ASCII string, and translated to ciphertext, number
 * by number, small letter by small letter, and capital letter by capital letter, 
 * and keep other characters intact.
 */
#define VP_POS_DATA_TYPE_ALPHANUMERIC_STRING  10

/** Pass the VP_POS_DATA_TYPE_BYTE_ARRAY value as the data type argument to the
 * VpPosEncryptCardData call to indicate that the data being encrypted is a
 * character array. A character is really a byte from 0x0 to 0xFF.
 */
#define VP_POS_DATA_TYPE_BYTE_ARRAY  11

/** The VpPosDate struct specifies the date used in the identity
 * for the IBE encryption of the FPE session key in the
 * encryption transmission block (ETB). Typically, this is the
 * current date.
 */
typedef struct VpPosDate
{
  int year;
  int month;
  int day;
} VpPosDate;

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify using POS 2.3.2 version.
 */
#define VP_POS_FLAGS_ENCRYPT_232 0x00000100

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify using POS 4.2 version.
 * It is the default verion as it is defined as 0.
 */
#define VP_POS_FLAGS_ENCRYPT_420 0x00000000

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the new Structure-Preserving Encryption
 * (SPE) mode. The behavior is as follows:
  * <p> For standalone PAN data, the PAN will be encrypted using the PEP2
 * algorithm.  The main difference between PEP2 and PEP1 is that PEP2
 * leaves some leading and trailing digits in the clear.  Refer to the 
 * IBKEEP document for full details on PEP2.
 * <p> For MIDs, they must be 4-23 digits long.
 * <p> For track 1 data, only the sensitive components will be encrypted,
 * instead of performing full encryption of track data. The encryption
 * is done using the 'TEP2' algorithm. Refer to the IBKEEP document 
 * for full details on TEP2. Note that it does slightly more stringent 
 * input validation than the full track encryption (TEP1), as it must parse 
 * parts of the track to encrypt. In particular, Track 1s must be a minimum
 * of 19 characters long and a maximum of 79 characters long, excluding a
 * null terminator. All Track 1 data that does not comply with these
 * requirements will result in an error.
 * <p> For track 2 data, only the sensitive components will be encrypted,
 * instead of performing full encryption of track data. The encryption
 * is done using the 'TEP2' algorithm. Refer to the IBKEEP document 
 * for full details on TEP2. Note that it does slightly more stringent 
 * input validation than the full track encryption (TEP1), as it must parse 
 * parts of the track to encrypt. In particular, Track 2s must be a minimum
 * of 16 characters long and a maximum of 40 characters long, excluding a
 * null terminator. All Track 2 data that does not comply with these
 * requirements will result in an error. 
 * <p> For Track 3, the encryption will be the same regardless of whether
 * this flag is set or not.
 * <p> NOTE: When using SPE on a PAN or Track 1 or Track 2 (which have
 * PANs inside them), the leading and trailing digits of the PAN will not
 * be encrypted. The POS will tweak with those digits, however, so two
 * PANs with different leading and trailing but the same middle digits
 * will have different ciphertexts.
 * <p> NOTE: If this flag is set on context creation, it will affect all
 * keys created with the context and all encryptions done with those keys. 
 * The ETBs generated will be different if this flag is set than if it is
 * not.
 */
#define VP_POS_FLAGS_ENCRYPT_SPE 0x00000001

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the new format preserving encryption
 * mode, which performs encryption of track data on the PAN and DD fileds
 * seperately, but keep other fields unchanged.
 */
#define VP_POS_FLAGS_ENCRYPT_FPE 0x00000002

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the old format preserving encryption
 * mode, which performs full encryption of track data.
*/
#define VP_POS_FLAGS_ENCRYPT_FULL 0

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the POS application would utilize a big
 * size of cache to achieve high speed BBX encryption.
 * This mode is recommended if the memory size on POS is big enough.
 */
#define VP_POS_FLAGS_HSPEED_HMEMORY 0x00000020

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the POS application would utilize a medium
 * size of cache to achieve relative high speed BBX encryption.
 * Set to this mode when the memory size on POS is relatively big.
 */
#define VP_POS_FLAGS_MSPEED_MMEMORY 0x00000010

/** Set this flag in the flags argument of the
 * VpPosCreateContext to specify the POS application would utilize a minimum
 * size of cache to achieve BBX encryption.
 * Set to this mode when the memory size on POS is relative small.
 */
#define VP_POS_FLAGS_LSPEED_LMEMORY 0x00000000

/** Set this flag in the argument of VpPosCreateContext
 *  to specify the POS application will use TerminalAuth
 *  This flag is set automatically when using the function VpPosCreateContextWithAuthInfo
 *  Note that Terminal Authentication should only be used with 3072 bit IBE keys.
 *  This corresponds to use of TEP1x, TEP2x and TEP4 algorithms (not TEP1 or TEP2).
*/
#define VP_POS_FLAGS_AUTHENTICATION_MODE 0x00000040

/** The VpPosCreateContext call creates the context used for subsequent
 * calls to the VpPosGenerateEtb, VpPosCreateKey and
 * VpPosEncryptCardData calls. This call creates the toolkit library
 * context and initializes it with the necessary support contexts.
 * Typically, this is called once at startup then the POS context
 * is kept around so you do not incur the cost of initializing
 * the toolkit for every operation. When you are done with the
 * POS context, use the VpPosDestroyContext call to destroy it and
 * the associated toolkit library context.
 * @param flags Used to indicate non-standard behavior for the
 * context. Unrecognized flags are ignored.
 * @param context The address of a VpPosContext variable
 * where the newly created POS context is placed.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosCreateContext(
  unsigned int flags,
  VpPosContext* context);

/** The VpPosDestroyContext call destroys a POS context created
 * with the VpPosCreateContext call. This call frees up the memory for the
 * context and destroys the associated toolkit library context.
 * Any VpPosKeys created with the POS context should be
 * destroyed before the POS context is destroyed.
 * @param posContext The address of the variable that stores
 * the pointer to the POS context. The POS context variable
 * is cleared to prevent dangling references to the destroyed
 * POS context.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosDestroyContext(
  VpPosContext* posContext);


/** The VpPosCreateContextWithAuthInfo call creates the POS context  
 *  needed for calls to VpPos<XXX> functions. This can be used to 
 *  instead of making call to VpPosCreateContext and VpPosSetAuthInfo
 *  @param flags Used to indicate non-standard behavior for the
 *  context. Unrecognized flags are ignored. If the Auth flag
 *  is not set, this function will set it.
 *  @param rSecret_16Bytes is the rSecret of the Terminal, needed for Terminal Auth
 *  Note that rSecret_16Bytes MUST be exactly 16 bytes of binary data.
 *  @param TID_16Bytes is the Terminal ID, needed for Terminal Auth
 *  Note that TID_16Bytes MUST be exactly 16 bytes of binary data.
 *  @param context The address of a VpPosContext variable
 *  where the newly created POS context is placed.
 *  @return 0 if success, otherwise a non-zero error code.
 */
int VpPosCreateContextWithAuthInfo(
  unsigned int flags, 
  const unsigned char* rSecret_16Bytes,
  const unsigned char* TID_16Bytes,
  VpPosContext *posContext);

/** The VpPosGenerateTerminalAuthSecret call is needed for support
 *  of the new TerminalAuth capabilities of v4.2
 *  This call generates the R-Secret which is a 16-byte value
 *  @param posContext The POS context to use
 *  @param rSecretBuffer_16Bytes, the address of a 16-byte buffer to hold the R-Secret value
 *  @return 0 if success, otherwise a  non-zero error code.
*/
int VpPosGenerateTerminalAuthSecret(VpPosContext posContext, unsigned char *rSecretBuffer_16Bytes);

/** The VpPosGenerateTerminalAuthIdentity call is needed for support
 *  of the new TerminalAuth capabilities of v4.2
 *  This call generates the TID which is a 16-byte value
 *  @param posContext The POS context to use
 *  @param TIDBuffer_16Bytes, the address of a 16-byte buffer to hold the TID value
 *  @return 0 if success, otherwise a  non-zero error code.
*/
int VpPosGenerateTerminalAuthIdentity(VpPosContext posContext, unsigned char *TIDBuffer_16Bytes);

/** The VpPosSetAuthInfo can be used to set or change the Terminal Auth info used with the posContext
 *  @param posContext The POS context to use
 *  @param rSecret_16Bytes, the rSecret to use with the provided posContext
 *  Note that rSecret_16Bytes MUST be exactly 16 bytes of binary data.
 *  @param TID_16Bytes, the Terminal ID to use with the provided posContext
 *  Note that TID_16Bytes MUST be exactly 16 bytes of binary data.
 *  @return 0 if success, otherwise a non-zero error code
*/
int VpPosSetAuthInfo(VpPosContext posContext, const unsigned char *rSecret_16Bytes, const unsigned char *TID_16Bytes);

/** The VpPosGenerateTerminalAuthVerificationHash call is needed for support of manually
 *  creating a displayable hash of the terminal's TID and RSecret
 *  @param key, the key to use for HMAC operation
 *  @param hashBuffer is a pointer to an allocated buffer in which the resulting
 *   hash will be placed.
 *  @param hashBufferLength should indicate the size of the buffer.
 *  @param outputLength will be filled in with the actual size of the hash.
 *  @return 0 if success, otherwise a non-zero error code
*/
int VpPosGenerateTerminalAuthVerificationHash(
  VpPosKey key,
  unsigned char *hashBuffer,
  unsigned int hashBufferLength,
  unsigned int *outputLength);

/** The VpPosSetAuthMode call is provided to allow changing the Auth mode for a POS context
 *  @param posContext, the POS context to modify
 *  @param mode, set to 0 to disable 'Auth' mode
 *               set to non-zero to enable 'Auth' mode
 *  @return 0 if success, otherwise a non-zero error code
*/
int VpPosSetAuthMode(VpPosContext posContext, int mode);

/** The VpPosGetFlags call is provided to request the flags applied to apllied to a POS context
 *  @param posContext, the POS context to query
 *  @param flags, flags that are applied to the POS context
 *  @return 0 if success, otherwise a non-zero error code
*/
int VpPosGetFlags(VpPosContext posContext, unsigned int* flags);

/**
 *
*/
int VpPosGenerateTerminalEnrollmentETB(
  VpPosKey key,
  const char *identityString,
  const VpPosDate *identityDate,
  const unsigned char *paramsData,
  const unsigned int paramsLength,
  unsigned char *outputBuffer,
  unsigned int bufferSize,
  unsigned int *outputLength);

/** The VpPosSeedRandom call adds seed material for the
 * random number generator used by the toolkit.
 *
 * You can configure the random number generator you want to use in the
 * PosConfig.h file. There are three options: PRNG, PRNG with protected
 * seed, or hardware RNG. The default is PRNG, which is a FIPS 186
 * PRNG. As per the standard, you must provide at least 32
 * bytes of seed before using the PRNG. The protected seed PRNG is
 * also based on FIPS 186, and requires the same seeding before use.
 * If you choose to use a hardware RNG, you must provide one
 * yourself, and then the behavior of this function is dictated
 * by that implementation.
 *
 * It is very important that good seed material is used or the
 * randomly generated FPE keys are vulnerable to attack.
 *
 * If the random number generator has not been seeded correctly,
 * subsequent calls that use the random number generator
 * such as VpPosCreateKey fail. You can call this function as many times as
 * you want to add additional seed material.
 * @param posContext The POS context to which seed material is being added.
 * @param seedData The buffer of seed data to use.
 * @param seedLength The length of the seed data buffer.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosSeedRandom(
  VpPosContext posContext,
  unsigned char* seedData,
  unsigned int seedLength);

/** The VpPosCreateKey call creates a new key to be used with
 * the VpPosGenerateEtb and VpPosEncryptCardData calls.
 * At the end of a crypto-period (typically daily) the key
 * should be destroyed using the VpPosDestroyKey call.
 * WARNING: A VpPosKey becomes invalid after the associated
 * VpPosContext is destroyed. You should destroy the key
 * using the VpPosDestroyKey call before destroying the VpPosContext.
 * @param posContext The POS context.
 * @param keyVersion A version number for the key. Key versions
 * must be unique within a given crypto-period, but are reused in other
 * crypto-periods. Typically, only a single key
 * is used in a given crypto-period.
 * @param key The address of a VpPosKey variable where the
 * newly-created key is placed.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosCreateKey(
  VpPosContext posContext,
  unsigned char keyVersion,
  VpPosKey* key);

/** The VpPosCreateKeyFromData call creates a new key that is used with
 * the VpPosGenerateEtb and VpPosEncryptCardData calls from
 * key data that was saved previously with the VpPosSaveKey call.
 * At the end of a crypto-period (typically daily) the key
 * should be destroyed using the VpPosDestroyKey call.
 * @param posContext The POS context.
 * @param keyData The data to use to construct the key. This should
 * be data that was saved previously using the VpPosSaveKey call.
 * @param keyDataLength The length of the data that keyData points to.
 * @param key The address of a VpPosKey variable where the
 * newly-created key is placed.
 * @param keyVersion The address of an unsigned char variable where
 * the key version of the saved key is placed.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosCreateKeyFromData(
  VpPosContext posContext,
  const unsigned char* keyData,
  unsigned int keyDataLength,
  VpPosKey* key,
  unsigned char* keyVersion);

/** The VpPosSaveKey call saves the key data associated with the
 * VpPosKey to a data buffer so it persists. The data can be
 * used to restore the key data later, for example, after a power
 * outage. This key data should not leave the secure processing
 * environment on the POS terminal. Therefore, the data should not be
 * exposed directly in the API available to the general
 * processing environment.
 * If the key data is successfully written to the buffer, then the
 * number of bytes written is returned in the keyDataLength argument.
 * If the key data buffer is not big enough (or is NULL), then the
 * required size of the buffer is returned in the keyDataLength
 * output argument and the VP_POS_ERROR_BUFFER_TOO_SMALL error is
 * returned.
 * @param key The opaque VtPosKey that contains the base session key.
 * @param keyData The buffer where the key data is written.
 * @param bufferSize The size of the data buffer.
 * @param keyDataLength The number of bytes written to the
 * key data buffer.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosSaveKey(
  VpPosKey key,
  unsigned char* keyData,
  unsigned int bufferSize,
  unsigned int* keyDataLength);

/** The VpPosDestroyKey call destroys a key created with the
 * VpPosCreateKey call. This operation should be called at the end of a
 * crypto-period when you are done with a key. It frees up the memory
 * allocated for the key data.
 * WARNING: Make sure that you destroy a given VpPosKey before
 * destroying the VpPosContext used to create it.
 * @param key The address of a VpPosKey variable that stores
 * the pointer to the key. The key variable is cleared to prevent
 * dangling references to the destroyed key.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosDestroyKey(
  VpPosKey* key);

/** The VpPosGenerateEtb call generates the encryption transmission block
 * (ETB) data, which contains the IBE-encrypted key data and any other
 * information (e.g. crypto protocol version) that is needed on the
 * host side for decryption. This block
 * of data is included in messages from the POS terminal to the
 * host and transmits the encryption/key information used to encrypt the
 * credit card numbers.
 * <p>Generating the ETB is a fairly expensive operation and takes at least
 * a few seconds, so you only want to generate it once at
 * the start of a new crypto-period when a new VpPosKey is
 * created and then cache the ETB data to be included in subsequent
 * authorization and batch update messages.
 * <p>To determine how big the output buffer needs to be, you can
 * call this function with an outputBuffer of NULL and an outputBufferSize of 0.
 * This configuration sets the outputLength argument to the size that the output buffer
 * needs to be and returns a VP_POS_ERROR_BUFFER_TOO_SMALL error.
 * You can then allocate the memory for the output buffer and call it
 * again with valid outputBuffer and outputBufferSize values.
 * @param key A key created with VpPosCreateKey.
 * @param identityString The common name of the identity used for
 * the IBE operation (e.g. test@sample.com). The domain name part
 * of the identity string usually matches the name of the
 * district used for the IBE operation.
 * @param identityDate The date included in the identity for the
 * IBE operation. This is usually the current date.
 * @param paramsData The district parameter data. This is the
 * data obtained from a district parameter update message from
 * the host to the POS terminal. This information is the compressed BB params
 * data obtained by running the BB params conversion tool on the
 * normal params data obtained from the Key Server.
 * @param paramsLength The length of the paramsData buffer.
 * @param outputBuffer The output buffer for the ETB data.
 * @param outputBufferSize The size of the output buffer.
 * @param outputLength The actual size of the ETB data written to
 * the output buffer (or how big the output buffer needs to be, if
 * the given output buffer is too small).
 * @return 0 if success, otherwise a non-zero error code.
 */
#if 0
int VpPosGenerateEtb(
 VpPosKey key,
 const char* identityString,
 const VpPosDate* identityDate,
 const unsigned char* paramsData,
 const unsigned int paramsLength,
 unsigned char* outputBuffer,
 unsigned int outputBufferSize,
 unsigned int* outputLength);
#else

#define VP_POS_ENC_ALG_TEP1_TEP2          1
#define VP_POS_ENC_ALG_TEP1x_TEP2x_TEP4   2

int SecureVpPosGenerateEtb(
  VpPosKey key,
  unsigned int Algorithm,
  const VpPosDate* identityDate,
  unsigned char* outputBuffer,
  unsigned int bufferSize,
  unsigned int* outputLength);
#endif
/** The VpPosCancelEtb call is invoked from an interrupt handler to
 * cancel a VpPosGenerateEtb that is in progress on the main execution thread.
 * After the interrupt handler exits and control resumes
 * in the VpPosGenerateEtb call a
 * VP_POS_ERROR_OPERATION_CANCELED error is returned.
 * @param key The key used for the VpPosGenerateEtb call that is
 * being canceled.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosCancelEtb(
  VpPosKey key);

/** The VpPosEncryptCardData call encrypts credit card information using
 * a key created by the VpPosCreateKey call. It encrypts PAN, MID, and
 * track data.
 * <p>To determine how big the output buffer needs to be use this
 * call with a NULL outputBuffer and an outputBufferSize of 0. In this configuration,
 * the outputLength is set to the size required for the output
 * buffer and a VP_POS_ERROR_BUFFER_TOO_SMALL error is returned.
 * You can then allocate the memory for the output buffer and call it
 * again with valid outputBuffer and outputBufferSize values.
 * @param key The key to use. The actual encryption key for the data
 * is derived from base key data for the VpPosKey using a key
 * derivation function (HMAC/AES). The derived keys are different for
 * each type of data (i.e. PAN, MID, or track data).
 * @param inputDataType The type of data being encrypted. This should
 * be one of the VP_POS_DATA_TYPE_XXX values.
 * @param inputData The input data. For PAN and MID data types the
 * input data is numeric ASCII characters and the output data is
 * also numeric ASCII characters. Track data contains the ASCII
 * equivalents of the appropriate alphabet for that track data and
 * ASCII characters in the base-64 encoding
 * alphabet. The track data includes the start and end sentinel bytes.
 * <p>The input data does not have to be null-terminated. If it is
 * null-terminated and you must include the null termination
 * character in the output data, then pass strlen+1 for the input data
 * length. The output data is also null-terminated and the
 * outputLength includes the null terminator.
 * @param inputLength The length of the input data.
 * @param outputBuffer The output buffer for the encrypted data.
 * @param outputBufferSize The size of the output buffer.
 * @param outputLength The actual size of the data written to
 * the output buffer.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosEncryptCardData(
  VpPosKey key,
  int inputDataType,
  const char* inputData,
  unsigned int inputLength,
  char* outputBuffer,
  unsigned int outputBufferSize,
  unsigned int* outputLength);

/** The VpPosGetVersionInfo call returns information about the POS API.
 * The crypto protocol version refers to the formats of the data
 * exchanged between the POS API and the Host API. This includes
 * the district parameters (PPUB), the encryption transmission block
 * (ETB), and the POS-encrypted credit card data. Whenever any of
 * these formats change, the crypto protocol version changes. The POS
 * application should call VpPosGetVersionInfo to get the
 * crypto protocol version and it should be included in the
 * messages that are sent to the host. On the host side the
 * crypto protocol version is passed to the TranslateCardData
 * function so the host API knows the format of the card data.
 * The POS version number and POS version string can be used by
 * the POS application to print debugging/diagnostic information
 * about the version of the POS API being used. This version
 * changes with every official release of the POS API.
 * <p>NOTE: The crypto protocol version may not change with a new POS API
 * version unless there was a change to the crypto data exchanged
 * between the POS terminal and the host.
 * <p>Any of the output
 * arguments can be set to NULL if the caller does not need to
 * retrieve that version info.
 * @param cryptoProtocolVersion Output argument for the crypto protocol
 * version.
 * @param posVersionNumber Output argument for the POS API version.
 * @param posVersionString Output argument for the POS API version string.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosGetVersionInfo(
  unsigned char* cryptoProtocolVersion,
  unsigned int* posVersionNumber,
  const char** posVersionString);

/** The VpPosGetSupportedCryptoProtocolVersions call returns the minimum and
 * maximum crypto protocol versions that this version of the POS can
 * support. When setting the POS API to a custom crypto protocol version,
 * ensure that it is within this range.
 * @param earliestSupportedCryptoProtocolVersion Output argument for the
 * earliest supported crypto protocol version.
 * @param latestSupportedCryptoProtocolVersion Output argument for the
 * latest supported crypto protocol version.
 * @return 0 if success, otherwise a non-zero error code.
 */
int VpPosGetSupportedCryptoProtocolVersions(
  unsigned char* earliestSupportedCryptoProtocolVersion,
  unsigned char* latestSupportedCryptoProtocolVersion);

/** The VpPosGetSupportedStatus call returns the information whether
 *  any of the new features in Terminal SDK is supported or not.
 *  The return code 1 means supported and 0 means unsurported.
 *  By design it does not support to set more than one flag at a time. If this happens,
 *  the code will take the order of precedence as follows:
 *  check if VP_POS_232_SUPPORT is set; if not, then check if VP_POS_TEP4_SUPPORT is
 *  set; if not, then check if VP_POS_STRINGS_SUPPORT is set. *  
 */
int VpPosGetSupportedStatus(
  int flag);

#define VP_POS_232_SUPPORT        0x00000001
#define VP_POS_TEP4_SUPPORT       0x00000010
#define VP_POS_STRINGS_SUPPORT    0x00000100

#define VP_POS_VERSION_1_0    0x00010000
#define VP_POS_VERSION_2_0    0x00020000
#define VP_POS_VERSION_2_1    0x00020100
#define VP_POS_VERSION_2_2    0x00020200
#define VP_POS_VERSION_2_2_1  0x00020201
#define VP_POS_VERSION_2_3    0x00020300
#define VP_POS_VERSION_2_3_1  0x00020301
#define VP_POS_VERSION_2_3_2  0x00020302
#define VP_POS_VERSION_3_1    0x00030100
#define VP_POS_VERSION_4_0    0x00040000
#define VP_POS_VERSION_4_1_0  0x00040100
#define VP_POS_VERSION_4_2_0  0x00040200

/** The VpPosGetCryptoProtocolVersion call gets the current
 * version used by a specific VpPosContext.
 *
 * @param posContext The POS context to check.
 * @param version The version of the POS context being used.
 */
int VpPosGetCryptoProtocolVersion(
  VpPosContext posContext,
  unsigned int *version);

/** The base of the POS error codes. 
 */
#define VP_POS_ERROR_BASE 10000

/** An invalid VpPosContext was supplied.
 */
#define VP_POS_ERROR_INVALID_POS_CONTEXT          VP_POS_ERROR_BASE

/** BB params malformed.
 */
#define VP_POS_ERROR_INVALID_PARAMS_DATA          VP_POS_ERROR_BASE+1

/** Error encoding the identity.
 */
#define VP_POS_ERROR_ENCODE_IDENTITY              VP_POS_ERROR_BASE+2

/** Out of memory.
 */
#define VP_POS_ERROR_OUT_OF_MEMORY                VP_POS_ERROR_BASE+3

/** Could not build BB params object from parameters.
 */
#define VP_POS_ERROR_CREATE_PARAMS_OBJECT         VP_POS_ERROR_BASE+4

/** Could not set BB params object with parameters.
 */
#define VP_POS_ERROR_SET_PARAMS_OBJECT            VP_POS_ERROR_BASE+5

/** Error creating the key.
 */
#define VP_POS_ERROR_CREATE_KEY                   VP_POS_ERROR_BASE+6

/** Error setting the public key.
 */
#define VP_POS_ERROR_SET_PUBLIC_KEY               VP_POS_ERROR_BASE+7

/** Error in BB encrypt.
 */
#define VP_POS_ERROR_BB_ENCRYPT                   VP_POS_ERROR_BASE+8

/** Invalid random object.
 */
#define VP_POS_ERROR_INVALID_RANDOM_OBJECT        VP_POS_ERROR_BASE+9

/** Error seeding the random object.
 */
#define VP_POS_ERROR_SEED_RANDOM                  VP_POS_ERROR_BASE+10

/** Buffer too small.
 */
#define VP_POS_ERROR_BUFFER_TOO_SMALL             VP_POS_ERROR_BASE+11

/** Unexpected null argument.
 */
#define VP_POS_ERROR_NULL_ARG                     VP_POS_ERROR_BASE+12

/** Error generating random bytes.
 */
#define VP_POS_ERROR_GENERATE_RANDOM_BYTES        VP_POS_ERROR_BASE+13

/** Insufficient random seed supplied.
 */
#define VP_POS_ERROR_INSUFFICIENT_RANDOM_SEED     VP_POS_ERROR_BASE+14

/** Bad VpPosKey given.
 */
#define VP_POS_ERROR_INVALID_KEY                  VP_POS_ERROR_BASE+15

/** Internal error.
 */
#define VP_POS_ERROR_TOOLKIT_ERROR                VP_POS_ERROR_BASE+16

/** Could not initialize for encrypting.
 */
#define VP_POS_ERROR_CARD_DATA_ENCRYPT_INIT       VP_POS_ERROR_BASE+17

/** Error on encrypting.
 */
#define VP_POS_ERROR_CARD_DATA_ENCRYPT            VP_POS_ERROR_BASE+18

/** Invalid data type.
 */
#define VP_POS_ERROR_INVALID_DATA_TYPE            VP_POS_ERROR_BASE+19

/** Could not create algorithm object with which to encrypt.
 */
#define VP_POS_ERROR_CREATE_ENCRYPT_ALGORITHM     VP_POS_ERROR_BASE+20

/** Could not create a VpPosKey from this data.
 */
#define VP_POS_ERROR_INVALID_KEY_DATA             VP_POS_ERROR_BASE+21

/** Key data format is too new.
 */
#define VP_POS_ERROR_KEY_DATA_FORMAT_TOO_NEW      VP_POS_ERROR_BASE+22

/** Identity string longer than 255 chars.
 */
#define VP_POS_ERROR_INVALID_IDENTITY_STRING      VP_POS_ERROR_BASE+23

/** BB params too old.
 */
#define VP_POS_ERROR_PARAMS_DATA_TOO_OLD          VP_POS_ERROR_BASE+24

/** BB params too new.
 */
#define VP_POS_ERROR_PARAMS_DATA_TOO_NEW          VP_POS_ERROR_BASE+25

/** User cancelled operation.
 */
#define VP_POS_ERROR_OPERATION_CANCELED           VP_POS_ERROR_BASE+26

/** Internal error.
 */
#define VP_POS_ERROR_INTERNAL_ERROR               VP_POS_ERROR_BASE+27

/** Card data does not correspond to proper format.
 */
#define VP_POS_ERROR_INVALID_CARD_DATA            VP_POS_ERROR_BASE+28

/** Invalid date used.
 */
#define VP_POS_ERROR_INVALID_IDENTITY_DATE        VP_POS_ERROR_BASE+29

/** Input longer than valid format.
 */
#define VP_POS_ERROR_INPUT_LENGTH_TOO_LONG        VP_POS_ERROR_BASE+30

/** Input shorter than valid format.
 */
#define VP_POS_ERROR_INPUT_LENGTH_TOO_SHORT       VP_POS_ERROR_BASE+31

/** Random seed contains an illegal pattern.
 */
#define VP_POS_ERROR_REPETITIVE_RANDOM_SEED       VP_POS_ERROR_BASE+32

/** Internal error.
 */
#define VP_POS_ERROR_CANNOT_SET_ALPHABET          VP_POS_ERROR_BASE+33

/** Internal error.
 */
#define VP_POS_ERROR_INVALID_RANDOM_TWEAK_OPTION  VP_POS_ERROR_BASE+34

/** Internal error.
 */
#define VP_POS_ERROR_INVALID_ALPHABET_RANGE       VP_POS_ERROR_BASE+35

/** Internal error.
 */
#define VP_POS_ERROR_ALPHABET_TOO_SHORT           VP_POS_ERROR_BASE+36

/** Unrecognized protocol version.
 */
#define VP_POS_ERROR_INVALID_PROTOCOL_VERSION     VP_POS_ERROR_BASE+37

/** Track 1 data has unsupported format code. (Only 'B' is supported.)
 */
#define VP_POS_ERROR_UNSUPPORTED_FORMAT_CODE      VP_POS_ERROR_BASE+38     

/** Invalid number of spaces in pan (either as standalone or in tracks 1/2).
 */
#define VP_POS_ERROR_SPACES_IN_PAN                VP_POS_ERROR_BASE+39

/** An illegal combination of flags were specified on context create.
 */
#define VP_POS_ERROR_ILLEGAL_CONTEXT_FLAGS        VP_POS_ERROR_BASE+40

/** Error base64 encoding a string.
 */
#define VP_POS_ERROR_BASE64_ENCODE                VP_POS_ERROR_BASE+41

/** Context lacks Terminal RSecret
 */
#define VP_POS_ERROR_NO_TERMINAL_RSECRET          VP_POS_ERROR_BASE+42

/** Context lacks Terminal ID
 */
#define VP_POS_ERROR_NO_TERMINAL_ID               VP_POS_ERROR_BASE+43

#ifdef INCLUDE_POS232
/** Error to invoke a function unsupported from POS 232.
 */
#define VP_POS_ERROR_UNSUPPORTED_232_CALL         VP_POS_ERROR_BASE+44
#endif

/** Params file not found.
 */
#define VP_POS_ERROR_NOT_FOUND_PARAMS_FILE        VP_POS_ERROR_BASE+45

/** Params file does not correspond to proper format.
 */
#define VP_POS_ERROR_INVALID_PARAMS_FILE          VP_POS_ERROR_BASE+46

/** Identity string not found in parameters file.
 */
#define VP_POS_ERROR_NOT_FOUND_IDENTITY_STRING    VP_POS_ERROR_BASE+47

/** Params data not found in parameters file.
 */
#define VP_POS_ERROR_NOT_FOUND_PARAMS_DATA        VP_POS_ERROR_BASE+48

/** Params file does not correspond to proper format.
 */
#define VP_POS_ERROR_INVALID_ENCRYPTION_ALGORITHM VP_POS_ERROR_BASE+49


#ifdef __cplusplus
};
#endif

#endif
