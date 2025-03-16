cmake_minimum_required(VERSION 3.16)

project(guardian LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(guardian
  main.cpp
  image.cpp image.h
  miniz/miniz.c miniz/miniz.h miniz/miniz_common.h miniz/miniz_tdef.c miniz/miniz_tdef.h miniz/miniz_tinfl.c miniz/miniz_tinfl.h miniz/miniz_zip.c miniz/miniz_zip.h
)

include(GNUInstallDirs)
install(TARGETS guardian
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
