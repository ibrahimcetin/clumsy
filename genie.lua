-- genie, https://github.com/bkaradzic/GENie
-- known working version
-- https://github.com/bkaradzic/bx/blob/51f25ba638b9cb35eb2ac078f842a4bed0746d56/tools/bin/windows/genie.exe

MINGW_ACTION = 'gmake'

if _ACTION == MINGW_ACTION then
    -- need a msys2 with clang
    premake.gcc.cc   = 'clang'
    premake.gcc.cxx  = 'clang++'
    premake.gcc.ar   = 'llvm-ar'
    premake.llvm = true

    local LIB_DIVERT_MINGW = 'external/WinDivert-2.2.0-A'
    local ROOT = _WORKING_DIR
    print(ROOT)

    solution('clumsy')
        location("./build")
        configurations({'Debug', 'Release'})
        platforms({'x32', 'x64'})

        project('clumsy')
            language("C")
            files({'src/**.c', 'src/**.h'})
            links({'WinDivert', 'comctl32', 'Winmm', 'ws2_32'})

            configuration('Debug')
                flags({'ExtraWarnings', 'Symbols'})
                defines({'_DEBUG'})
                kind("ConsoleApp")

            configuration('Release')
                flags({"Optimize"})
                flags({'Symbols'}) -- keep the debug symbols for development
                defines({'NDEBUG'})
                kind("ConsoleApp")

            configuration(MINGW_ACTION)
                links({'kernel32', 'gdi32', 'comdlg32', 'uuid', 'ole32'}) -- additional libs
                buildoptions({
                    '-Wno-missing-braces',
                    '-Wno-missing-field-initializers',
                    '--std=c99'
                })
                objdir('obj_'..MINGW_ACTION)

            configuration({'x32', MINGW_ACTION})
                defines({'X32'}) -- defines would be passed to resource compiler for whatever reason
                includedirs({LIB_DIVERT_MINGW .. '/include'})
                libdirs({LIB_DIVERT_MINGW .. '/x86'})
                resoptions({'-O coff', '-F pe-i386'}) -- mingw64 defaults to x64

            configuration({'x64', MINGW_ACTION})
                defines({'X64'})
                includedirs({LIB_DIVERT_MINGW .. '/include'})
                libdirs({LIB_DIVERT_MINGW .. '/x64'})

            local function set_bin(platform, config, arch)
                local platform_str = platform

                local subdir = ROOT .. '/bin/' .. platform_str .. '/' .. config .. '/' .. arch
                local divert_lib

                if arch == 'x64' then
                    divert_lib = ROOT .. '/' .. LIB_DIVERT_MINGW .. '/x64/'
                else
                    divert_lib = ROOT .. '/' .. LIB_DIVERT_MINGW .. '/x86/'
                end

                configuration({platform, config, arch})
                    targetdir(subdir)
                    debugdir(subdir)

                    postbuildcommands({
                        -- robocopy returns non 0 will fail make
                        'cp ' .. divert_lib .. "WinDivert* " .. subdir,
                        'cp ' .. ROOT .. "/etc/config.txt " .. subdir,
                    })
            end

            set_bin(MINGW_ACTION, 'Debug', "x32")
            set_bin(MINGW_ACTION, 'Debug', "x64")
            set_bin(MINGW_ACTION, 'Release', "x32")
            set_bin(MINGW_ACTION, 'Release', "x64")
end

