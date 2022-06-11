# 
#  Created on Fri Jun 10 2022
# 
#  Copyright (C) SplitGemini
#  Author: SplitGemini
# 

find_library(BASS_LIBRARY NAMES bass HINTS ${CMAKE_SOURCE_DIR}/bass
             PATHS ${CMAKE_SOURCE_DIR}/bass)

if (BASS_LIBRARY)
    set(BASS_LIBRARY_FOUND TRUE)
    add_library(libbass SHARED IMPORTED GLOBAL)
    set_target_properties(libbass PROPERTIES
        IMPORTED_LOCATION "${BASS_LIBRARY}"
        IMPORTED_NO_SONAME 1)
endif()
