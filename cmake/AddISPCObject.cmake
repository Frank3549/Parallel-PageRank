#  Adapted from ISPC example code, Intel copyright message reproduced below.
#
#  Copyright (c) 2018-2019, Intel Corporation
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of Intel Corporation nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
#   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
#   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Compile ISPC file populating NAME_OBJECTS in parent context with parent files
function(add_ISPC_object)
  set(oneValueArgs NAME SRC_FILE TARGET)
  set(multiValueArgs FLAGS)
  cmake_parse_arguments("ISPC" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  get_filename_component(ISPC_SRC_NAME "${ISPC_SRC_FILE}" NAME_WE)
  set(ISPC_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ISPC_SRC_NAME}_ispc.h")
  set(ISPC_OBJ_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ISPC_SRC_NAME}_ispc${CMAKE_CXX_OUTPUT_EXTENSION}")

  add_custom_command(
    OUTPUT ${ISPC_OBJ_FILE} ${ISPC_HEADER_FILE}
    COMMAND
    ${ISPC_EXECUTABLE}
    ${ISPC_FLAGS}
    --target=${ISPC_TARGET}
    "${CMAKE_CURRENT_SOURCE_DIR}/${ISPC_SRC_FILE}"
    -h ${ISPC_HEADER_FILE}
    -o ${ISPC_OBJ_FILE}
    DEPENDS ${ISPC_EXECUTABLE}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ISPC_SRC_FILE}"
  )

  set("${ISPC_NAME}_OBJECTS" ${ISPC_OBJ_FILE} PARENT_SCOPE)
endfunction()
