#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/action.o \
	${OBJECTDIR}/src/file_utils.o \
	${OBJECTDIR}/src/logger.o \
	${OBJECTDIR}/src/sleep_utils.o \
	${OBJECTDIR}/src/config_utils.o \
	${OBJECTDIR}/src/string_utils.o \
	${OBJECTDIR}/src/daemon_utils.o \
	${OBJECTDIR}/src/queral.o \
	${OBJECTDIR}/src/queue.o \
	${OBJECTDIR}/src/memcached_client.o \
	${OBJECTDIR}/src/curl_client.o


# C Compiler Flags
CFLAGS=-lev -lpthread -lcurl -lconfig -lmemcached

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/queral

dist/Debug/GNU-Linux-x86/queral: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/queral ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/action.o: src/action.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/action.o src/action.c

${OBJECTDIR}/src/file_utils.o: src/file_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/file_utils.o src/file_utils.c

${OBJECTDIR}/src/logger.o: src/logger.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/logger.o src/logger.c

${OBJECTDIR}/src/sleep_utils.o: src/sleep_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/sleep_utils.o src/sleep_utils.c

${OBJECTDIR}/src/config_utils.o: src/config_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/config_utils.o src/config_utils.c

${OBJECTDIR}/src/string_utils.o: src/string_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/string_utils.o src/string_utils.c

${OBJECTDIR}/src/daemon_utils.o: src/daemon_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/daemon_utils.o src/daemon_utils.c

${OBJECTDIR}/src/queral.o: src/queral.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/queral.o src/queral.c

${OBJECTDIR}/src/queue.o: src/queue.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/queue.o src/queue.c

${OBJECTDIR}/src/memcached_client.o: src/memcached_client.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/memcached_client.o src/memcached_client.c

${OBJECTDIR}/src/curl_client.o: src/curl_client.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -Isrc -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/curl_client.o src/curl_client.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/queral

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
