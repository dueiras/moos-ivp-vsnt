#--------------------------------------------------------
# The CMakeLists.txt for:                       iSerial
# Author(s):                              Douglas Lima
#--------------------------------------------------------

SET(SRC
  SerialV23.cpp
  SerialV23_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iSerialV23 ${SRC})

TARGET_LINK_LIBRARIES(iSerialV23
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

