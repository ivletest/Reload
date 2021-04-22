add_library("cJSON" STATIC
  "cJSON.c"
  "cJSON.h"
  "cJSON_Utils.c"
  "cJSON_Utils.h"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("cJSON" PROPERTIES
    OUTPUT_NAME "cJSON"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Debug/cJSON"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Debug/cJSON"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Debug/cJSON"
  )
endif()
target_include_directories("cJSON" PRIVATE
  $<$<CONFIG:Debug>:/home/ivan/Source/Repos/Reload/third_party/common/cJSON>
)
target_compile_definitions("cJSON" PRIVATE
)
target_link_directories("cJSON" PRIVATE
)
target_link_libraries("cJSON"
)
target_compile_options("cJSON" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
target_link_options("cJSON" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("cJSON" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("cJSON" PROPERTIES
    OUTPUT_NAME "cJSON"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Release/cJSON"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Release/cJSON"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/cJSON/build/bin/linux/Release/cJSON"
  )
endif()
target_include_directories("cJSON" PRIVATE
  $<$<CONFIG:Release>:/home/ivan/Source/Repos/Reload/third_party/common/cJSON>
)
target_compile_definitions("cJSON" PRIVATE
)
target_link_directories("cJSON" PRIVATE
)
target_link_libraries("cJSON"
)
target_compile_options("cJSON" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
target_link_options("cJSON" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("cJSON" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()