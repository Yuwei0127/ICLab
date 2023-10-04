/****************************************************************************
 * Project : PKCS#11 v2.0: HSP4000 Slot
 * Workfile: pkcs11x.h
 * Original Author: Andrew O'Marah
 * Description: 
 * 
 * This header file contains extensions to the standard (e.g. additional mechanisms
 * or structures).
 *
 * (C) Copyright 1996-1999 Baltimore Technologies (UK) Ltd.
 *
 * Language:    Microsoft Visual C++ V5.0
 * Target:      PC Compatible Intel 386 based. 
 *              Windows 95/Windows NT
 *
 * $Id: //hhperf1/Categories/pkcs11v2.01/010406HSMPKCS11Adaptor/010426WinNT/v3/r5/pkcs11v2.01/dev/FrameWork/inc/pkcs11x.h#1 $
 ****************************************************************************/

#ifndef __PKCS_11_X_H_ 
#define __PKCS_11_X_H_ 

#include "pkcs11.h"

    // Store current packing alignment.
    #if defined (WIN32)
        #pragma pack( push, Cryptoki_Extensions, 1 )
    #endif //WIN32

    #ifdef  __cplusplus
	    extern "C"
	    {
    #endif

    /* Imports an object that has been exported. hSession is the handle to the session that will
       receive the key, pExportedKey is a pointer to the buffer containing the exported key,
       usExportedObjectLen is the length of the buffer, and phObject is a pointer to the handle
       that receives the import object handle.
       Return values: CKR_OK, CKR_DEVICE_ERROR, CKR_SESSION_HANDLE_INVALID, CKR_DEVICE_REMOVED,
        CKR_OPERATION_ACTIVE, CKR_KEY_TYPE_INCONSISTENT, CKR_DEVICE_MEMORY, CKR_ATTRIBUTE_VALUE_INVALID,
        CKR_HOST_MEMORY */
    CK_DECLARE_FUNCTION( CK_RV, C_ImportObject )
    (
        CK_SESSION_HANDLE hSession,
	    CK_BYTE_PTR pExportedObject,
		CK_ULONG ulExportedObjectLen,
		CK_OBJECT_HANDLE_PTR phObject
    );

    /* Exports an object and deletes it from the session. hSession is the sessions handle, hObject 
       is the handle of the object that is to be exported, pExportedKey is a pointer to a CK_BYTE 
       buffer that receives the exported object and pusExportedObjectLen points to a CK_USHORT 
       that receives the buffer length.
       Return values: CKR_OK, CKR_DEVICE_ERROR, CKR_SESSION_HANDLE_INVALID, CKR_DEVICE_REMOVED,
        CKR_OPERATION_ACTIVE, CKR_KEY_TYPE_INCONSISTENT  */

    CK_DECLARE_FUNCTION( CK_RV, C_ExportObject )
    ( 
        CK_SESSION_HANDLE hSession,
        CK_OBJECT_HANDLE hObject,
        CK_BYTE_PTR pExportedObject,
        CK_ULONG_PTR pusExportedObjectLen
    );

    // Mechanisms.
    #define CKM_SPKM_DERIVE_MD5        CKM_VENDOR_DEFINED + 0x0002
    #define CKM_SPKM_DERIVE_SHA1       CKM_VENDOR_DEFINED + 0x0003

    /* Definition for use in SPKM key derivation. */
    typedef struct CK_SPKM_DERIVE_PARAMS 
    {
        CK_CHAR cUsage;            /* 'C' or 'I' */
	    CK_CHAR ccAlgorithmNumber; /* 0 to 9 */
    } CK_SPKM_DERIVE_PARAMS;

    #ifdef __cplusplus
	    } // extern "C"
    #endif

    // Restore packing alignment.
    #if defined (WIN32)
        #pragma pack( pop, Cryptoki_Extensions )
    #endif //WIN32

#endif /* __PKCS_11_X_H_  */
