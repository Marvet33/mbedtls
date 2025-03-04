/**
 * \file mbedtls/config_adjust_legacy_crypto.h
 * \brief Adjust legacy configuration configuration
 *
 * Automatically enable certain dependencies. Generally, MBEDLTS_xxx
 * configurations need to be explicitly enabled by the user: enabling
 * MBEDTLS_xxx_A but not MBEDTLS_xxx_B when A requires B results in a
 * compilation error. However, we do automatically enable certain options
 * in some circumstances. One case is if MBEDTLS_xxx_B is an internal option
 * used to identify parts of a module that are used by other module, and we
 * don't want to make the symbol MBEDTLS_xxx_B part of the public API.
 * Another case is if A didn't depend on B in earlier versions, and we
 * want to use B in A but we need to preserve backward compatibility with
 * configurations that explicitly activate MBEDTLS_xxx_A but not
 * MBEDTLS_xxx_B.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MBEDTLS_CONFIG_ADJUST_LEGACY_CRYPTO_H
#define MBEDTLS_CONFIG_ADJUST_LEGACY_CRYPTO_H

/* Auto-enable MBEDTLS_MD_LIGHT based on MBEDTLS_MD_C.
 * This allows checking for MD_LIGHT rather than MD_LIGHT || MD_C.
 */
#if defined(MBEDTLS_MD_C)
#define MBEDTLS_MD_LIGHT
#endif

/* Auto-enable MBEDTLS_MD_LIGHT if needed by a module that didn't require it
 * in a previous release, to ensure backwards compatibility.
 */
#if defined(MBEDTLS_ECJPAKE_C) || \
    defined(MBEDTLS_PEM_PARSE_C) || \
    defined(MBEDTLS_ENTROPY_C) || \
    defined(MBEDTLS_PK_C) || \
    defined(MBEDTLS_PKCS12_C) || \
    defined(MBEDTLS_RSA_C) || \
    defined(MBEDTLS_SSL_TLS_C) || \
    defined(MBEDTLS_X509_USE_C) || \
    defined(MBEDTLS_X509_CREATE_C)
#define MBEDTLS_MD_LIGHT
#endif

/* MBEDTLS_ECP_LIGHT is auto-enabled by the following symbols:
 * - MBEDTLS_ECP_C because now it consists of MBEDTLS_ECP_LIGHT plus functions
 *   for curve arithmetic. As a consequence if MBEDTLS_ECP_C is required for
 *   some reason, then MBEDTLS_ECP_LIGHT should be enabled as well.
 * - MBEDTLS_PK_PARSE_EC_EXTENDED and MBEDTLS_PK_PARSE_EC_COMPRESSED because
 *   these features are not supported in PSA so the only way to have them is
 *   to enable the built-in solution.
 *   Both of them are temporary dependencies:
 *   - PK_PARSE_EC_EXTENDED will be removed after #7779 and #7789
 *   - support for compressed points should also be added to PSA, but in this
 *     case there is no associated issue to track it yet.
 * - PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE because Weierstrass key derivation
 *   still depends on ECP_LIGHT.
 * - PK_C + USE_PSA + PSA_WANT_ALG_ECDSA is a temporary dependency which will
 *   be fixed by #7453.
 */
#if defined(MBEDTLS_ECP_C) || \
    defined(MBEDTLS_PK_PARSE_EC_EXTENDED) || \
    defined(MBEDTLS_PK_PARSE_EC_COMPRESSED) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_DERIVE)
#define MBEDTLS_ECP_LIGHT
#endif

/* MBEDTLS_PK_PARSE_EC_COMPRESSED is introduced in MbedTLS version 3.5, while
 * in previous version compressed points were automatically supported as long
 * as PK_PARSE_C and ECP_C were enabled. As a consequence, for backward
 * compatibility, we auto-enable PK_PARSE_EC_COMPRESSED when these conditions
 * are met. */
#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_ECP_C)
#define MBEDTLS_PK_PARSE_EC_COMPRESSED
#endif

/* Helper symbol to state that there is support for ECDH, either through
 * library implementation (ECDH_C) or through PSA. */
#if (defined(MBEDTLS_USE_PSA_CRYPTO) && defined(PSA_WANT_ALG_ECDH)) || \
    (!defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_ECDH_C))
#define MBEDTLS_CAN_ECDH
#endif

/* PK module can achieve ECDSA functionalities by means of either software
 * implementations (ECDSA_C) or through a PSA driver. The following defines
 * are meant to list these capabilities in a general way which abstracts how
 * they are implemented under the hood. */
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_ECDSA_C)
#define MBEDTLS_PK_CAN_ECDSA_SIGN
#define MBEDTLS_PK_CAN_ECDSA_VERIFY
#endif /* MBEDTLS_ECDSA_C */
#else /* MBEDTLS_USE_PSA_CRYPTO */
#if defined(PSA_WANT_ALG_ECDSA)
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
#define MBEDTLS_PK_CAN_ECDSA_SIGN
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC */
#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
#define MBEDTLS_PK_CAN_ECDSA_VERIFY
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */
#endif /* PSA_WANT_ALG_ECDSA */
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#if defined(MBEDTLS_PK_CAN_ECDSA_VERIFY) || defined(MBEDTLS_PK_CAN_ECDSA_SIGN)
#define MBEDTLS_PK_CAN_ECDSA_SOME
#endif

/* If MBEDTLS_PSA_CRYPTO_C is defined, make sure MBEDTLS_PSA_CRYPTO_CLIENT
 * is defined as well to include all PSA code.
 */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#define MBEDTLS_PSA_CRYPTO_CLIENT
#endif /* MBEDTLS_PSA_CRYPTO_C */

/* The PK wrappers need pk_write functions to format RSA key objects
 * when they are dispatching to the PSA API. This happens under USE_PSA_CRYPTO,
 * and also even without USE_PSA_CRYPTO for mbedtls_pk_sign_ext(). */
#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_RSA_C)
#define MBEDTLS_PK_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_PARSE_C
#endif

/* Helper symbol to state that the PK module has support for EC keys. This
 * can either be provided through the legacy ECP solution or through the
 * PSA friendly MBEDTLS_PK_USE_PSA_EC_DATA (see pk.h for its description). */
#if defined(MBEDTLS_ECP_C) || \
    (defined(MBEDTLS_USE_PSA_CRYPTO) && defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY))
#define MBEDTLS_PK_HAVE_ECC_KEYS
#endif /* MBEDTLS_PK_USE_PSA_EC_DATA || MBEDTLS_ECP_C */

#endif /* MBEDTLS_CONFIG_ADJUST_LEGACY_CRYPTO_H */
