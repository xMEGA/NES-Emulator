import qbs 1.0
import qbs.TextFile
import qbs.FileInfo

Project {
    name: "nes"
    property string platform: qbs.getEnv( "PLATFORM" );
    property path ThisPojectRoot: sourceDirectory + "../../../../"

    CppApplication {
        condition: platform == "linux_x86_64"

        type: "application"
        consoleApplication: false

        name: "nes_" + platform + " "

        files: [
            ThisPojectRoot + "src/main.cpp",
            ThisPojectRoot + "src/Emulator.cpp",
            ThisPojectRoot + "src/AudioDac/AudioDac.cpp",
            ThisPojectRoot + "src/Display/Display.cpp",
            ThisPojectRoot + "src/FileManager/FileOpenDialog.cpp",
            ThisPojectRoot + "src/FileManager/RomFileManager.cpp",
            ThisPojectRoot + "src/InputManager/InputManager.cpp",
            ThisPojectRoot + "src/Utils/File.cpp",
            ThisPojectRoot + "src/Utils/TimeCounter.cpp",

            ThisPojectRoot + "src/GameConsole/GameConsole.cpp",

            ThisPojectRoot + "src/GameConsole/Apu/Apu.cpp",
            //ThisPojectRoot + "src/GameConsole/Apu/*",

            ThisPojectRoot + "src/GameConsole/Apu/ApuRegisters.cpp",
            ThisPojectRoot + "src/GameConsole/Apu/NoiseGenerator.cpp",
            ThisPojectRoot + "src/GameConsole/Apu/SquareGenerator.cpp",
            ThisPojectRoot + "src/GameConsole/Apu/TriangleGenerator.cpp",

            ThisPojectRoot + "src/GameConsole/Cartridge/GameRomAnaliser.cpp",

            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/AoRom.cpp",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/IMapper.cpp",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/Mmc1.cpp",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/Mmc3.cpp",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/NoRom.cpp",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/UnRom.cpp",

            ThisPojectRoot + "src/GameConsole/Control/Control.cpp",
            ThisPojectRoot + "src/GameConsole/Cpu/Cpu.cpp",
            ThisPojectRoot + "src/GameConsole/Cpu/CpuOpCodes.cpp",
            ThisPojectRoot + "src/GameConsole/Cpu/CpuRegisters.cpp",

            ThisPojectRoot + "src/GameConsole/Ppu/Ppu.cpp",
            ThisPojectRoot + "src/GameConsole/Ppu/PpuPalette.cpp",
        ]

        cpp.includePaths: [
            ThisPojectRoot + "thirdparty/SDL2/build/"+ platform + "/include/SDL2/",
            ThisPojectRoot + "src/AudioDac/",
            ThisPojectRoot + "src/Display/",
            ThisPojectRoot + "src/FileManager/",
            ThisPojectRoot + "src/InputManager/",
            ThisPojectRoot + "src/Utils/",
            ThisPojectRoot + "src/GameConsole/",
            ThisPojectRoot + "src/GameConsole/Apu/",
            ThisPojectRoot + "src/GameConsole/Cartridge/",
            ThisPojectRoot + "src/GameConsole/Cartridge/Mappers/",
            ThisPojectRoot + "src/GameConsole/Control/",
            ThisPojectRoot + "src/GameConsole/Cpu/",
            ThisPojectRoot + "src/GameConsole/Ppu/",
         ]

        cpp.warningLevel: "all"
        cpp.optimization: "fast"
        cpp.debugInformation: true
        //cpp.cxxLanguageVersion: "c++11"

//        cpp.defines: [
//
//        ]

        cpp.commonCompilerFlags: [
            //'-fno-strict-aliasing',
            '-Wextra',
            '-Wall',
            '-O3',
            '-includeMacros.h'
        ]

        cpp.dynamicLibraries: [
            'pthread',
            'dl'
        ]

        cpp.staticLibraries: [
            'SDL2',
            'SDL2main'
        ]

        cpp.linkerFlags: [
            '-Wall',
        ]

        cpp.libraryPaths: [
             ThisPojectRoot + "thirdparty/SDL2/build/" + platform + "/lib64/"
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.installDir: ThisPojectRoot
            qbs.install: true

        }
    }
} 
