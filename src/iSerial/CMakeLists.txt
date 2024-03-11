#--------------------------------------------------------
# The CMakeLists.txt for:                       iSerial
# Author(s):                              Douglas Lima, Eduardo Eiras
#--------------------------------------------------------

SET(SRC
  Serial.cpp
  Serial_Info.cpp
  main.cpp
)

# Specify the path to the LibSerial library and include directory
set(LIBSERIAL_LIBRARY "/usr/lib/x86_64-linux-gnu/libserial.so")
set(LIBSERIAL_INCLUDE_DIR "/usr/include")

ADD_EXECUTABLE(iSerial ${SRC})

# Link against LibSerial
target_include_directories(iSerial PRIVATE ${LIBSERIAL_INCLUDE_DIR})
target_link_libraries(iSerial ${LIBSERIAL_LIBRARY})

TARGET_LINK_LIBRARIES(iSerial
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

