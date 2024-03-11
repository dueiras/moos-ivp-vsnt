cmake_minimum_required(VERSION 3.1.0)

project(pSimpass VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
     wsock32 )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m
      pthread )
endif (${WIN32})


SET(SRC
   Console.cpp 
   SIMPASS.cpp
   LatLong.cpp
   main.cpp
   MathUtils.cpp
   moosconnect.cpp
   NetworkClient.cpp
   NetworkManager.cpp
)  

ADD_EXECUTABLE(pSimpass ${SRC})

target_include_directories(pSimpass PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Encontre o pacote Qt5 e defina as variáveis necessárias
find_package(Qt5 REQUIRED COMPONENTS Core Network Gui)

# Adicione os diretórios de inclusão do Qt5 ao seu projeto
include_directories(${Qt5_INCLUDE_DIRS})

# Defina as bibliotecas Qt5 que seu projeto irá linkar
set(QT_LIBRARIES Qt5::Core Qt5::Network Qt5::Gui)


TARGET_LINK_LIBRARIES(pSimpass
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   apputil
   mbutil
   ${SYSTEM_LIBS} 
   ${QT_LIBRARIES} )

#Find MOOSGeodesy
find_package(MOOSGeodesy)
include_directories(${MOOSGeodesy_INCLUDE_DIRS} ${MOOSGeodesy_LIBRARIES})