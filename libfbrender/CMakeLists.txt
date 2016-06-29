SET(LIBFBRENDER_SRCLIST 
    vector4.cpp
    matrix4.cpp
    transform.cpp
    render/render_device.cpp
    render/fb_render_device.cpp)

FILE(GLOB_RECURSE LIBFBRENDER_HDRLIST ../include/*.h)

SOURCE_GROUP("Header Files" FILES ${LIBFBRENDER_HDRLIST})

SET(LIBRARIES )

ADD_LIBRARY(libfbrender ${LIBFBRENDER_SRCLIST} ${LIBFBRENDER_HDRLIST})

SET_TARGET_PROPERTIES(libfbrender PROPERTIES OUTPUT_NAME "fbrender") 

TARGET_LINK_LIBRARIES(libfbrender ${LIBRARIES})
INSTALL(TARGETS libfbrender ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

