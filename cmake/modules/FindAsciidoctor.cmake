find_package(PackageHandleStandardArgs)

find_program(ASCIIDOCTOR_EXECUTABLE NAMES asciidoctor)
if(ASCIIDOCTOR_EXECUTABLE)

  function(add_asciidoctor_manpages_target _target _out_var)
    set(_man_files)

    foreach(_source IN LISTS ARGN)
      string(REGEX REPLACE "\.adoc$" "" _name ${_source})
      get_filename_component(_name ${_name} NAME)
      set(_output ${CMAKE_CURRENT_BINARY_DIR}/${_name})

      add_custom_command(
        COMMAND ${ASCIIDOCTOR_EXECUTABLE} -b manpage ${_source} -o ${_output}
        OUTPUT ${_output}
        DEPENDS ${_source}
        COMMENT "Generating manpage ${_name}"
        VERBATIM)

      list(APPEND _man_files ${_output})
    endforeach()

    add_custom_target(${_target} ALL DEPENDS ${_man_files})

    set(${_out_var} ${_man_files} PARENT_SCOPE)
  endfunction()

endif()

find_package_handle_standard_args(Asciidoctor DEFAULT_MSG ASCIIDOCTOR_EXECUTABLE)
