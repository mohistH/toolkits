cmake_minimum_required(VERSION 3.15)

# project(demo_qt LANGUAGES CXX)
project(oct_toolkits VERSION 1.0.0)


# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# message("${CMAKE_MODULE_PATH}")
# FIND_PACKAGE(abc REQUIRED)

option(lib_is_shared                    "lib is a shared librry ?  true = yes, false = nah"         ON)



# ---------------------------------------------------------------------------------------------
set(publish_path  ${CMAKE_CURRENT_SOURCE_DIR}/publish/toolkits)

# ---------------------------------------------------------------------------------------------
# 64位
if (CMAKE_CL_64)
    # 设置 可执行程序输出目录
	set(publish_bin_debug 			${publish_path}/x64/bin/debug)
	set(publish_bin_release 		${publish_path}/x64/bin/release)

    # 设置库文件输出目录
	set(publish_lib_debug 			${publish_path}/x64/lib/debug)
	set(publish_lib_release 		${publish_path}/x64/lib/release)

    set(publih_lib_include          ${publish_path}/x64/include)

	set(PROJECT_NAME 				${PROJECT_NAME}64)
# ---------------------------------------------------------------------------------------------
# 32
else()
 # 设置 可执行程序输出目录
	set(publish_bin_debug 			${publish_path}/x86/bin/debug)
	set(publish_bin_release 		${publish_path}/x86/bin/release)

 # 设置库文件输出目录
	set(publish_lib_debug 			${publish_path}/x86/lib/debug)
	set(publish_lib_release 		${publish_path}/x86/lib/release)

    set(publih_lib_include          ${publish_path}/x86/include)
endif()
# ---------------------------------------------------------------------------------------------


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 				${publish_lib_debug})    
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 				${publish_lib_release})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 				${publish_bin_debug})   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 				${publish_bin_release}) 

# ---------------------------------------------------------------------------------------------
# windows创建库
function (lib_is_on_win)

    # 动态库还是静态库? 
    if (lib_is_shared)
        set(lib_type SHARED)
    else()
        set(lib_type STATIC)
    endif (lib_is_shared)




    # debug模式下增加后缀D
    set(CMAKE_DEBUG_POSTFIX "D") 



    # 个人更习惯下面的方式
    file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
    file(GLOB_RECURSE inc_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

    # 创建库
    add_library(${PROJECT_NAME} ${lib_type} ${src_files} ${inc_files}  )

    # 指定头文件搜索路径
    target_include_directories(${PROJECT_NAME}      PRIVATE         ${CMAKE_CURRENT_SOURCE_DIR}/include)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/oct_toolkits.h DESTINATION ${publih_lib_include})


    # 链接三方库
    if (utils_has_found)
        target_include_directories(${PROJECT_NAME}  PRIVATE ${calc_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME}       PRIVATE debug ${calc_debug_list_libs} optimized ${calc_release_list_libs})
    endif(utils_has_found)

endfunction(lib_is_on_win)

# ---------------------------------------------------------------------------------------------
# windows创建库
function (lib_is_on_linux)

    # 动态库还是静态库? 
    if (lib_is_shared)
        set(lib_type SHARED)
    else()
        set(lib_type STATIC)
    endif (lib_is_shared)




    # debug模式下增加后缀D
    set(CMAKE_DEBUG_POSTFIX "D") 



    # 个人更习惯下面的方式
    file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
    file(GLOB_RECURSE inc_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

    # 创建库
    add_library(${PROJECT_NAME} ${lib_type} ${src_files} ${inc_files}  )

    # 指定头文件搜索路径
    target_include_directories(${PROJECT_NAME}      PRIVATE         ${CMAKE_CURRENT_SOURCE_DIR}/include)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/oct_toolkits.h DESTINATION ${publih_lib_include})


    # 链接三方库
    if (utils_has_found)
        target_include_directories(${PROJECT_NAME}  PRIVATE ${calc_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME}       PRIVATE debug ${calc_debug_list_libs} optimized ${calc_release_list_libs})
    endif(utils_has_found)

endfunction(lib_is_on_linux)





# ---------------------------------------------------------------
# 创建demo， 调用动态库
function (lib_exam)
    set (exam_src 
            ${CMAKE_CURRENT_SOURCE_DIR}/example/main.cc
    )

    set (exam_inc 
         ${CMAKE_CURRENT_SOURCE_DIR}/include/oct_toolkits.h
    )

    # 创建
    set(exam_name "call_lib")
    add_executable(${exam_name} ${exam_inc} ${exam_src})

     # 指定头文件搜索路径
    target_include_directories(${exam_name}      PRIVATE            ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # 链接库
    target_link_libraries(${exam_name}           PRIVATE            ${PROJECT_NAME}  )

endfunction (lib_exam)





# ---------------------------------------------------------------
# 创建项目
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    lib_is_on_linux()
    lib_exam()
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    lib_is_on_win()
    lib_exam()

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
