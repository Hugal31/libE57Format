# SPDX-License-Identifier: MIT
# Copyright © Andy Maloney <asmaloney@gmail.com>

# CRCpp from here: https://github.com/d-bahr/CRCpp

target_compile_definitions( ${PROJECT_NAME}
    PRIVATE
        CRCPP_USE_CPP11
        CRCPP_BRANCHLESS
    )

target_sources( ${PROJECT_NAME}
	PRIVATE
	    ${CMAKE_CURRENT_LIST_DIR}/inc/CRC.h
)

target_include_directories( ${PROJECT_NAME}
	PRIVATE
	    ${CMAKE_CURRENT_LIST_DIR}/inc
)
