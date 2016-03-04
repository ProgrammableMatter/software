# @author: Raoul Rubien 2011

# show stats about binary
add_custom_command (TARGET ${BINARY}
         POST_BUILD
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMENT "${BINARY} built for [${CMCU}]@[${CDEFS}]:"
         COMMAND echo "MD5" && md5sum ./${BINARY} | cut -d' ' -f1
         COMMAND echo "filesize on system" && ls -l ./${BINARY} | cut -d' ' -f5 && echo ""
         COMMAND avr-size --format=sysv ${BINARY}
)

