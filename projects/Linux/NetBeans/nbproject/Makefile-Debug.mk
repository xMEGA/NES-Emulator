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
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/374577674/AudioDac.o \
	${OBJECTDIR}/_ext/425842872/Display.o \
	${OBJECTDIR}/_ext/1386528437/Emulator.o \
	${OBJECTDIR}/_ext/837277161/FileManager.o \
	${OBJECTDIR}/_ext/404011650/Apu.o \
	${OBJECTDIR}/_ext/404011650/ApuRegisters.o \
	${OBJECTDIR}/_ext/404011650/NoiseGenerator.o \
	${OBJECTDIR}/_ext/404011650/SquareGenerator.o \
	${OBJECTDIR}/_ext/404011650/TriangleGenerator.o \
	${OBJECTDIR}/_ext/488225511/Cartridge.o \
	${OBJECTDIR}/_ext/488225511/GameRomAnaliser.o \
	${OBJECTDIR}/_ext/503038730/AoRom.o \
	${OBJECTDIR}/_ext/503038730/IMapper.o \
	${OBJECTDIR}/_ext/503038730/Mmc1.o \
	${OBJECTDIR}/_ext/503038730/Mmc3.o \
	${OBJECTDIR}/_ext/503038730/NoRom.o \
	${OBJECTDIR}/_ext/503038730/UnRom.o \
	${OBJECTDIR}/_ext/1707403335/Control.o \
	${OBJECTDIR}/_ext/404013572/Cpu.o \
	${OBJECTDIR}/_ext/404013572/CpuOpCodes.o \
	${OBJECTDIR}/_ext/404013572/CpuRegisters.o \
	${OBJECTDIR}/_ext/1541642379/GameConsole.o \
	${OBJECTDIR}/_ext/404026065/Ppu.o \
	${OBJECTDIR}/_ext/404026065/PpuPalette.o \
	${OBJECTDIR}/_ext/404026065/PpuRegisters.o \
	${OBJECTDIR}/_ext/1514345027/InputManager.o \
	${OBJECTDIR}/_ext/1386528437/main.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../thirdparty/SDL2/build/linux_x86_64/lib64

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../build/Linux/Debug/emulator

${CND_DISTDIR}/../../../../build/Linux/Debug/emulator: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../build/Linux/Debug
	${LINK.cc} -o ${CND_DISTDIR}/../../../../build/Linux/Debug/emulator ${OBJECTFILES} ${LDLIBSOPTIONS} -lSDL2 -ldl -lpthread

${OBJECTDIR}/_ext/374577674/AudioDac.o: ../../../src/AudioDac/AudioDac.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/374577674
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/374577674/AudioDac.o ../../../src/AudioDac/AudioDac.cpp

${OBJECTDIR}/_ext/425842872/Display.o: ../../../src/Display/Display.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/425842872
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/425842872/Display.o ../../../src/Display/Display.cpp

${OBJECTDIR}/_ext/1386528437/Emulator.o: ../../../src/Emulator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1386528437/Emulator.o ../../../src/Emulator.cpp

${OBJECTDIR}/_ext/837277161/FileManager.o: ../../../src/FileManager/FileManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/837277161
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/837277161/FileManager.o ../../../src/FileManager/FileManager.cpp

${OBJECTDIR}/_ext/404011650/Apu.o: ../../../src/GameConsole/Apu/Apu.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404011650
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404011650/Apu.o ../../../src/GameConsole/Apu/Apu.cpp

${OBJECTDIR}/_ext/404011650/ApuRegisters.o: ../../../src/GameConsole/Apu/ApuRegisters.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404011650
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404011650/ApuRegisters.o ../../../src/GameConsole/Apu/ApuRegisters.cpp

${OBJECTDIR}/_ext/404011650/NoiseGenerator.o: ../../../src/GameConsole/Apu/NoiseGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404011650
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404011650/NoiseGenerator.o ../../../src/GameConsole/Apu/NoiseGenerator.cpp

${OBJECTDIR}/_ext/404011650/SquareGenerator.o: ../../../src/GameConsole/Apu/SquareGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404011650
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404011650/SquareGenerator.o ../../../src/GameConsole/Apu/SquareGenerator.cpp

${OBJECTDIR}/_ext/404011650/TriangleGenerator.o: ../../../src/GameConsole/Apu/TriangleGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404011650
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404011650/TriangleGenerator.o ../../../src/GameConsole/Apu/TriangleGenerator.cpp

${OBJECTDIR}/_ext/488225511/Cartridge.o: ../../../src/GameConsole/Cartridge/Cartridge.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/488225511
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/488225511/Cartridge.o ../../../src/GameConsole/Cartridge/Cartridge.cpp

${OBJECTDIR}/_ext/488225511/GameRomAnaliser.o: ../../../src/GameConsole/Cartridge/GameRomAnaliser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/488225511
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/488225511/GameRomAnaliser.o ../../../src/GameConsole/Cartridge/GameRomAnaliser.cpp

${OBJECTDIR}/_ext/503038730/AoRom.o: ../../../src/GameConsole/Cartridge/Mappers/AoRom.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/AoRom.o ../../../src/GameConsole/Cartridge/Mappers/AoRom.cpp

${OBJECTDIR}/_ext/503038730/IMapper.o: ../../../src/GameConsole/Cartridge/Mappers/IMapper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/IMapper.o ../../../src/GameConsole/Cartridge/Mappers/IMapper.cpp

${OBJECTDIR}/_ext/503038730/Mmc1.o: ../../../src/GameConsole/Cartridge/Mappers/Mmc1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/Mmc1.o ../../../src/GameConsole/Cartridge/Mappers/Mmc1.cpp

${OBJECTDIR}/_ext/503038730/Mmc3.o: ../../../src/GameConsole/Cartridge/Mappers/Mmc3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/Mmc3.o ../../../src/GameConsole/Cartridge/Mappers/Mmc3.cpp

${OBJECTDIR}/_ext/503038730/NoRom.o: ../../../src/GameConsole/Cartridge/Mappers/NoRom.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/NoRom.o ../../../src/GameConsole/Cartridge/Mappers/NoRom.cpp

${OBJECTDIR}/_ext/503038730/UnRom.o: ../../../src/GameConsole/Cartridge/Mappers/UnRom.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/503038730
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/503038730/UnRom.o ../../../src/GameConsole/Cartridge/Mappers/UnRom.cpp

${OBJECTDIR}/_ext/1707403335/Control.o: ../../../src/GameConsole/Control/Control.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1707403335
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1707403335/Control.o ../../../src/GameConsole/Control/Control.cpp

${OBJECTDIR}/_ext/404013572/Cpu.o: ../../../src/GameConsole/Cpu/Cpu.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404013572
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404013572/Cpu.o ../../../src/GameConsole/Cpu/Cpu.cpp

${OBJECTDIR}/_ext/404013572/CpuOpCodes.o: ../../../src/GameConsole/Cpu/CpuOpCodes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404013572
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404013572/CpuOpCodes.o ../../../src/GameConsole/Cpu/CpuOpCodes.cpp

${OBJECTDIR}/_ext/404013572/CpuRegisters.o: ../../../src/GameConsole/Cpu/CpuRegisters.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404013572
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404013572/CpuRegisters.o ../../../src/GameConsole/Cpu/CpuRegisters.cpp

${OBJECTDIR}/_ext/1541642379/GameConsole.o: ../../../src/GameConsole/GameConsole.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1541642379
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1541642379/GameConsole.o ../../../src/GameConsole/GameConsole.cpp

${OBJECTDIR}/_ext/404026065/Ppu.o: ../../../src/GameConsole/Ppu/Ppu.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404026065
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404026065/Ppu.o ../../../src/GameConsole/Ppu/Ppu.cpp

${OBJECTDIR}/_ext/404026065/PpuPalette.o: ../../../src/GameConsole/Ppu/PpuPalette.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404026065
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404026065/PpuPalette.o ../../../src/GameConsole/Ppu/PpuPalette.cpp

${OBJECTDIR}/_ext/404026065/PpuRegisters.o: ../../../src/GameConsole/Ppu/PpuRegisters.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/404026065
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/404026065/PpuRegisters.o ../../../src/GameConsole/Ppu/PpuRegisters.cpp

${OBJECTDIR}/_ext/1514345027/InputManager.o: ../../../src/InputManager/InputManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1514345027
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1514345027/InputManager.o ../../../src/InputManager/InputManager.cpp

${OBJECTDIR}/_ext/1386528437/main.o: ../../../src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../src/Source -I../../src/Source/GameConsole -I../../../thirdparty/SDL2/build/linux_x86_64/include/SDL2 -std=c++98 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1386528437/main.o ../../../src/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/../../../../build/Linux/Debug/emulator

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
