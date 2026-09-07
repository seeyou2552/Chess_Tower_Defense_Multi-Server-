
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was mysql-connector-cpp-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(Protobuf CONFIG)
if(NOT "ON")
    find_dependency(Threads)
    find_dependency(OpenSSL)
    find_dependency(RapidJSON CONFIG)
    find_dependency(ZLIB)
    find_dependency(lz4)
    find_dependency(zstd)
    if("OFF")
        find_dependency(unofficial-libmysql)
    endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/unofficial-mysql-connector-cpp-targets.cmake")

if(NOT UNOFFICIAL_MYSQL_CONNECTOR_CPP_INITIALIZED)
    if(NOT "ON")
        set_target_properties(unofficial::mysql-connector-cpp::connector PROPERTIES
            INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:Threads::Threads>;$<LINK_ONLY:OpenSSL::SSL>;$<LINK_ONLY:rapidjson>;$<LINK_ONLY:ZLIB::ZLIB>;$<LINK_ONLY:>;$<LINK_ONLY:lz4::lz4>;$<LINK_ONLY:zstd::libzstd>"
        )
        # Cf. mysql-concpp-config.cmake.in
        if(WIN32)
            set_property(TARGET unofficial::mysql-connector-cpp::connector APPEND PROPERTY INTERFACE_LINK_LIBRARIES "dnsapi")
        elseif(NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
            set_property(TARGET unofficial::mysql-connector-cpp::connector APPEND PROPERTY INTERFACE_LINK_LIBRARIES "resolv;dl")
        endif()
        if("OFF")
            set_target_properties(unofficial::mysql-connector-cpp::connector-jdbc PROPERTIES
                INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:unofficial::libmysql::libmysql>"
            )
            # Cf. jdbc/driver/CMakeLists.txt
            if(WIN32)
                set_property(TARGET unofficial::mysql-connector-cpp::connector-jdbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES "dnsapi")
            elseif(NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
                set_property(TARGET unofficial::mysql-connector-cpp::connector-jdbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES "resolv;dl")
            endif()
        endif()
    endif()
    set_property(TARGET unofficial::mysql-connector-cpp::connector APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:protobuf::libprotobuf-lite>"
    )
    set(UNOFFICIAL_MYSQL_CONNECTOR_CPP_INITIALIZED 1 CACHE INTERNAL "")
endif()

check_required_components(mysql-connector-cpp)
