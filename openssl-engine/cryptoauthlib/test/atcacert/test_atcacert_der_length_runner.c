/**
 * \file
 *
 * \copyright Copyright (c) 2017 Microchip Technology Inc. and its subsidiaries (Microchip). All rights reserved.
 *
 * \page License
 *
 * You are permitted to use this software and its derivatives with Microchip
 * products. Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Microchip may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with a
 *    Microchip integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "test/unity.h"
#include "test/unity_fixture.h"

#ifdef __GNUC__
// Unity macros trigger this warning
#pragma GCC diagnostic ignored "-Wnested-externs"
#endif

TEST_GROUP_RUNNER(atcacert_der_enc_length)
{
    RUN_TEST_CASE(atcacert_der_enc_length, short_form);
    RUN_TEST_CASE(atcacert_der_enc_length, long_form_2byte);
    RUN_TEST_CASE(atcacert_der_enc_length, long_form_3byte);
    RUN_TEST_CASE(atcacert_der_enc_length, long_form_4byte);
    RUN_TEST_CASE(atcacert_der_enc_length, long_form_5byte);
    RUN_TEST_CASE(atcacert_der_enc_length, small_buf);
    RUN_TEST_CASE(atcacert_der_enc_length, bad_params);
}

TEST_GROUP_RUNNER(atcacert_der_dec_length)
{
    RUN_TEST_CASE(atcacert_der_dec_length, good);
    RUN_TEST_CASE(atcacert_der_dec_length, zero_size);
    RUN_TEST_CASE(atcacert_der_dec_length, not_enough_data);
    RUN_TEST_CASE(atcacert_der_dec_length, indefinite_form);
    RUN_TEST_CASE(atcacert_der_dec_length, too_large);
    RUN_TEST_CASE(atcacert_der_dec_length, bad_params);
}