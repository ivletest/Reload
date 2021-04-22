add_library("fmt" STATIC
      "include/fmt/chrono.h"
      "include/fmt/color.h"
      "include/fmt/compile.h"
      "include/fmt/core.h"
      "include/fmt/format-inl.h"
      "include/fmt/format.h"
      "include/fmt/locale.h"
      "include/fmt/os.h"
      "include/fmt/ostream.h"
      "include/fmt/posix.h"
      "include/fmt/printf.h"
      "include/fmt/ranges.h"
    "src/format.cc"
    "src/os.cc"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("fmt" PROPERTIES
    OUTPUT_NAME "fmt"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Debug/fmt"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Debug/fmt"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Debug/fmt"
  )
endif()
target_include_directories("fmt" PRIVATE
  $<$<CONFIG:Debug>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/include>
  $<$<CONFIG:Debug>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/src>
)
target_compile_definitions("fmt" PRIVATE
  $<$<CONFIG:Debug>:FMT_OS>
)
target_link_directories("fmt" PRIVATE
)
target_link_libraries("fmt"
)
target_compile_options("fmt" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
target_link_options("fmt" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("fmt" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("fmt" PROPERTIES
    OUTPUT_NAME "fmt"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Release/fmt"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Release/fmt"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/fmt/build/bin/linux/Release/fmt"
  )
endif()
target_include_directories("fmt" PRIVATE
  $<$<CONFIG:Release>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/include>
  $<$<CONFIG:Release>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/src>
)
target_compile_definitions("fmt" PRIVATE
  $<$<CONFIG:Release>:FMT_OS>
)
target_link_directories("fmt" PRIVATE
)
target_link_libraries("fmt"
)
target_compile_options("fmt" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++17>
)
target_link_options("fmt" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("fmt" PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()