# ------------------------------------------------------------------------------ 
# Set compiler flags
# ------------------------------------------------------------------------------

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckFortranCompilerFlag)
function(set_compiler_flag _result _lang)
  # build a list of flags from the arguments
  set(_list_of_flags)
  # also figure out whether the function
  # is required to find a flag
  set(_flag_is_required FALSE)
  foreach(_arg IN ITEMS ${ARGN})
    string(TOUPPER "${_arg}" _arg_uppercase)
    if(_arg_uppercase STREQUAL "REQUIRED")
      set(_flag_is_required TRUE)
    else()
      list(APPEND _list_of_flags "${_arg}")
    endif()
  endforeach()

  set(_flag_found FALSE)
  # loop over all flags, try to find the first which works
  foreach(flag IN ITEMS ${_list_of_flags})

    unset(_flag_works CACHE)
    if(_lang STREQUAL "C")
      check_c_compiler_flag("${flag}" _flag_works)
    elseif(_lang STREQUAL "CXX")
      check_cxx_compiler_flag("${flag}" _flag_works)
    elseif(_lang STREQUAL "Fortran")
      check_Fortran_compiler_flag("${flag}" _flag_works)
    else()
      message(FATAL_ERROR "Unknown language in set_compiler_flag: ${_lang}")
    endif()

    # if the flag works, use it, and exit
    # otherwise try next flag
    if(_flag_works)
      set(${_result} "${flag}" PARENT_SCOPE)
      set(_flag_found TRUE)
      break()
    endif()
  endforeach()

  # raise an error if no flag was found
  if(_flag_is_required AND NOT _flag_found)
    message(FATAL_ERROR "None of the required flags were supported")
  endif()
endfunction()

# ------------------------------------------------------------------------------ 
# Add headers and sources recursively
# ------------------------------------------------------------------------------

if( APPLE )
	macro( add_recursive dir retVal )
		file( GLOB_RECURSE ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c ${dir}/*.mm ${dir}/*.m )
	endmacro()
else()
	macro( add_recursive dir retVal )
		file( GLOB_RECURSE ${retVal} ${dir}/*.h ${dir}/*.cpp ${dir}/*.c )
	endmacro()
endif()