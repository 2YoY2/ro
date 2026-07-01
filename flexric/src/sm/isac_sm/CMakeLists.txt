# SPDX-License-Identifier: LicenseRef-CSSL-1.0

cmake_minimum_required(VERSION 3.15) # setting this is required
project(isac_sm)            # this sets the project name

set(SM_ENCODING_ISAC_SRC
                      ../sm_proc_data.c 
                      isac_sm_ric.c 
                      isac_sm_agent.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                     # This dependency sucks!
                     $<TARGET_OBJECTS:e2ap_ran_func_obj>
                     ie/isac_data_ie.c
                      )

if(SM_ENCODING_ISAC STREQUAL "PLAIN")
  set(SM_ENCODING_ISAC_SRC_ASN 
    ${SM_ENCODING_ISAC_SRC}
    enc/isac_enc_plain.c 
    dec/isac_dec_plain.c 
    )

  # Shared
  add_library(isac_sm SHARED ${SM_ENCODING_ISAC_SRC_ASN} )
  target_compile_options(isac_sm PRIVATE -fPIC -fvisibility=hidden)

  # Static
  add_library(isac_sm_static STATIC ${SM_ENCODING_ISAC_SRC_ASN})

elseif(SM_ENCODING_ISAC STREQUAL "ASN")
  message(FATAL_ERROR "ISAC SM ASN not implemented")

  add_subdirectory(ie/asn)

  add_library(isac_sm SHARED
                      ${SM_ENCODING_ISAC_SRC}
                      enc/isac_enc_asn.c 
                      dec/isac_dec_asn.c 
                      )
                    #target_include_directories(isac_sm PRIVATE
                    #        "ie/asn")
                    #  target_compile_options(isac_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
                    #  target_compile_options(isac_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)



elseif(SM_ENCODING_ISAC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "ISAC SM FB not implemented")
  add_library(isac_sm SHARED 
                      ${SM_ENCODING_ISAC_SRC}
                      enc/isac_enc_fb.c 
                      dec/isac_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type")
endif()

# ToDo: Dependency from KPM_VERSION needs to be removed!
target_compile_definitions(isac_sm PUBLIC ${SM_ENCODING_ISAC} ${E2AP_VERSION}  ${KPM_VERSION} )
target_compile_definitions(isac_sm_static PUBLIC ${SM_ENCODING_ISAC} ${E2AP_VERSION}  ${KPM_VERSION} )


