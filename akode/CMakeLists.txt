#################################################
#
#  (C) 2015 Slávek Banko
#  slavek (DOT) banko (AT) axis.cz
#
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################


#### pkg-config #################################

set( prefix ${CMAKE_INSTALL_PREFIX} )
string( REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}" "\${prefix}"
        exec_prefix ${EXEC_INSTALL_PREFIX} )
string( REGEX REPLACE "^${EXEC_INSTALL_PREFIX}" "\${exec_prefix}"
        libdir ${LIB_INSTALL_DIR} )
string( REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}" "\${prefix}"
        includedir ${INCLUDE_INSTALL_DIR} )

configure_file( akode-config.in akode-config @ONLY )
install( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/akode-config
         DESTINATION ${BIN_INSTALL_DIR} )

configure_file( akode.pc.in akode.pc @ONLY )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/akode.pc
         DESTINATION ${PKGCONFIG_INSTALL_DIR} )


##### build #####################################

tde_auto_add_subdirectories()

