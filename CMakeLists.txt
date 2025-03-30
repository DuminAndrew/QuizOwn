cmake_minimum_required(VERSION 3.16)
project(QuizOwn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Настройки для установщика
set(CPACK_PACKAGE_NAME "QuizOwn")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Quiz Application")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QuizOwn")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
set(CPACK_NSIS_ENABLE_UNINSTALL "ON")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "CreateShortCut \\\"$DESKTOP\\\\QuizOwn.lnk\\\" \\\"$INSTDIR\\\\bin\\\\QuizOwn.exe\\\"")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\QuizOwn.lnk\\\"")

include(CPack)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/quizmanager.cpp
    src/sectiondialog.cpp
    src/logger.cpp
)

set(HEADERS
    include/mainwindow.h
    include/quizmanager.h
    include/sectiondialog.h
    include/logger.h
)

set(RESOURCE_FILES
    resources/resources.qrc
)

qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES} ${RESOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

# Создаем директорию для ресурсов
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/resource) 