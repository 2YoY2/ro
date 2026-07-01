# SPDX-License-Identifier: LicenseRef-CSSL-1.0

set(SM_ENCODING_ISAC "PLAIN" CACHE STRING "The ISAC SM encoding to use")
set_property(CACHE SM_ENCODING_ISAC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected ISAC SM_ENCODING: ${SM_ENCODING_ISAC}")

if(SM_ENCODING_ISAC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_isac_sm
                      main.c 
                      ../../rnd/fill_rnd_data_isac.c
                      ../../../src/util/time_now_us.c
                      )              
elseif(SM_ENCODING_ISAC STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")
elseif(SM_ENCODING_ISAC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")
endif()

target_compile_definitions(test_isac_sm PUBLIC ${SM_ENCODING_ISAC})
target_link_libraries(test_isac_sm PUBLIC isac_sm_static -pthread)

enable_testing()
add_test(Unit_test_ISAC_SM test_isac_sm)
