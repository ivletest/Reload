add_library("spdlog" STATIC
      "include/spdlog/async.h"
      "include/spdlog/async_logger-inl.h"
      "include/spdlog/async_logger.h"
        "include/spdlog/cfg/argv.h"
        "include/spdlog/cfg/env.h"
        "include/spdlog/cfg/helpers-inl.h"
        "include/spdlog/cfg/helpers.h"
      "include/spdlog/common-inl.h"
      "include/spdlog/common.h"
        "include/spdlog/details/backtracer-inl.h"
        "include/spdlog/details/backtracer.h"
        "include/spdlog/details/circular_q.h"
        "include/spdlog/details/console_globals.h"
        "include/spdlog/details/file_helper-inl.h"
        "include/spdlog/details/file_helper.h"
        "include/spdlog/details/fmt_helper.h"
        "include/spdlog/details/log_msg-inl.h"
        "include/spdlog/details/log_msg.h"
        "include/spdlog/details/log_msg_buffer-inl.h"
        "include/spdlog/details/log_msg_buffer.h"
        "include/spdlog/details/mpmc_blocking_q.h"
        "include/spdlog/details/null_mutex.h"
        "include/spdlog/details/os-inl.h"
        "include/spdlog/details/os.h"
        "include/spdlog/details/periodic_worker-inl.h"
        "include/spdlog/details/periodic_worker.h"
        "include/spdlog/details/registry-inl.h"
        "include/spdlog/details/registry.h"
        "include/spdlog/details/synchronous_factory.h"
        "include/spdlog/details/tcp_client-windows.h"
        "include/spdlog/details/tcp_client.h"
        "include/spdlog/details/thread_pool-inl.h"
        "include/spdlog/details/thread_pool.h"
        "include/spdlog/details/windows_include.h"
        "include/spdlog/fmt/bin_to_hex.h"
        "include/spdlog/fmt/chrono.h"
        "include/spdlog/fmt/fmt.h"
        "include/spdlog/fmt/ostr.h"
      "include/spdlog/formatter.h"
      "include/spdlog/fwd.h"
      "include/spdlog/logger-inl.h"
      "include/spdlog/logger.h"
      "include/spdlog/pattern_formatter-inl.h"
      "include/spdlog/pattern_formatter.h"
        "include/spdlog/sinks/android_sink.h"
        "include/spdlog/sinks/ansicolor_sink-inl.h"
        "include/spdlog/sinks/ansicolor_sink.h"
        "include/spdlog/sinks/base_sink-inl.h"
        "include/spdlog/sinks/base_sink.h"
        "include/spdlog/sinks/basic_file_sink-inl.h"
        "include/spdlog/sinks/basic_file_sink.h"
        "include/spdlog/sinks/daily_file_sink.h"
        "include/spdlog/sinks/dist_sink.h"
        "include/spdlog/sinks/dup_filter_sink.h"
        "include/spdlog/sinks/hourly_file_sink.h"
        "include/spdlog/sinks/msvc_sink.h"
        "include/spdlog/sinks/null_sink.h"
        "include/spdlog/sinks/ostream_sink.h"
        "include/spdlog/sinks/ringbuffer_sink.h"
        "include/spdlog/sinks/rotating_file_sink-inl.h"
        "include/spdlog/sinks/rotating_file_sink.h"
        "include/spdlog/sinks/sink-inl.h"
        "include/spdlog/sinks/sink.h"
        "include/spdlog/sinks/stdout_color_sinks-inl.h"
        "include/spdlog/sinks/stdout_color_sinks.h"
        "include/spdlog/sinks/stdout_sinks-inl.h"
        "include/spdlog/sinks/stdout_sinks.h"
        "include/spdlog/sinks/syslog_sink.h"
        "include/spdlog/sinks/systemd_sink.h"
        "include/spdlog/sinks/tcp_sink.h"
        "include/spdlog/sinks/win_eventlog_sink.h"
        "include/spdlog/sinks/wincolor_sink-inl.h"
        "include/spdlog/sinks/wincolor_sink.h"
      "include/spdlog/spdlog-inl.h"
      "include/spdlog/spdlog.h"
      "include/spdlog/stopwatch.h"
      "include/spdlog/tweakme.h"
      "include/spdlog/version.h"
    "src/async.cpp"
    "src/cfg.cpp"
    "src/color_sinks.cpp"
    "src/file_sinks.cpp"
    "src/spdlog.cpp"
    "src/stdout_sinks.cpp"
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_dependencies("spdlog"
    "fmt"
  )
  set_target_properties("spdlog" PROPERTIES
    OUTPUT_NAME "spdlog"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Debug/spdlog"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Debug/spdlog"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Debug/spdlog"
  )
endif()
target_include_directories("spdlog" PRIVATE
  $<$<CONFIG:Debug>:/home/ivan/Source/Repos/Reload/third_party/common/spdlog/include>
  $<$<CONFIG:Debug>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/include>
)
target_compile_definitions("spdlog" PRIVATE
  $<$<CONFIG:Debug>:SPDLOG_FMT_EXTERNAL>
  $<$<CONFIG:Debug>:SPDLOG_COMPILED_LIB>
)
target_link_directories("spdlog" PRIVATE
)
target_link_libraries("spdlog"
  $<$<CONFIG:Debug>:fmt>
)
target_compile_options("spdlog" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++11>
)
target_link_options("spdlog" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("spdlog" PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_dependencies("spdlog"
    "fmt"
  )
  set_target_properties("spdlog" PROPERTIES
    OUTPUT_NAME "spdlog"
    ARCHIVE_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Release/spdlog"
    LIBRARY_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Release/spdlog"
    RUNTIME_OUTPUT_DIRECTORY "/home/ivan/Source/Repos/Reload/third_party/common/spdlog/build/bin/linux/Release/spdlog"
  )
endif()
target_include_directories("spdlog" PRIVATE
  $<$<CONFIG:Release>:/home/ivan/Source/Repos/Reload/third_party/common/spdlog/include>
  $<$<CONFIG:Release>:/home/ivan/Source/Repos/Reload/third_party/common/fmt/include>
)
target_compile_definitions("spdlog" PRIVATE
  $<$<CONFIG:Release>:SPDLOG_FMT_EXTERNAL>
  $<$<CONFIG:Release>:SPDLOG_COMPILED_LIB>
)
target_link_directories("spdlog" PRIVATE
)
target_link_libraries("spdlog"
  $<$<CONFIG:Release>:fmt>
)
target_compile_options("spdlog" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++11>
)
target_link_options("spdlog" PRIVATE
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("spdlog" PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE False
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()