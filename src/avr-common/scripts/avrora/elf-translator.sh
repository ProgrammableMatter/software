#!/bin/bash

#
# @author Raoul Rubien 2016
#

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

java -jar $DIR/elf-translator.jar $@
