cmake_minimum_required(VERSION 3.1)

set (CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules ${CMAKE_SOURCE_DIR}/cmake/Modules)
set (CMAKE_CXX_STANDARD 14)

message ("CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")

project(fontdef)

find_package(OpenCV REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_package(Freetype REQUIRED)
find_package(FreeImage REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${FreeImage_INCLUDE_DIRS})

add_definitions(-DBOOST_ALL_DYN_LINK)

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x601)
endif(WIN32)

file(GLOB HEADERS_FILES "*.h")
aux_source_directory(. SOURCE_FILES)
set(SOURCES ${HEADERS_FILES} ${SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	pthread 
	${Boost_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${FreeImage_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

