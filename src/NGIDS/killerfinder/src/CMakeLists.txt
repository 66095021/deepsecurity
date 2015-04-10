
SET(SOURCE_FILES
     main.cpp
     CPacketHandler.cpp
     CMain.cpp
     CObject.cpp
     CTcpSession.cpp
     CConfig.cpp
     CLog.cpp
     CL2Info.cpp
     CL3Info.cpp
     CL4Info.cpp
     CIpAddress.cpp
     CTcpSessionId.cpp
     CTcpSessionPool.cpp
     CTcpSessionPlugin.cpp
     CHttpInfo.cpp
     CHttpTxnDecoder.cpp
     CLock.cpp
     CTxnDecodeThread.cpp
     CPacketCaptureThread.cpp
     CSmtpTxnDecoder.cpp
     CMailInfo.cpp
     CPacketCreater.cpp
     CDBManager.cpp
     )


INCLUDE_DIRECTORIES(
    /usr/include
    /usr/local/include
    /usr/local/libxml2
    ${PROJET_SOURCE_DIR}/src
    )


LINK_DIRECTORIES(  
    /lib
    /usr/lib
    /usr/local/lib
    )

ADD_EXECUTABLE(killer-finder  ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES(killer-finder libpcap.so libxml2.so libhttp_parser.so libpthread.so libvmime.so libnet.so)

INSTALL(TARGETS killer-finder RUNTIME DESTINATION bin)

