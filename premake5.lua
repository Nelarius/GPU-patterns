
workspace "turing-patterns"
    if _ACTION then
        location("build")
    end

    configurations { "Debug", "Release", "Test" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "configurations:Test"
        defines { "DEBUG"}
        optimize "On"

    project "app"
        language "C++"
        targetdir "bin"
        files { "src/**.cpp", "src/**.h", "extern/json11/json11.cpp", "extern/imgui/**.cpp", "data/**.glsl", "**.json" }
        includedirs {
            "src", "extern", "extern/assimp/include", "extern/SDL/include", "extern/glew-1.13.0/include"
        }
        --This should be enabled once all VC warnings get fixed:
        --flags { "FatalWarnings" } // This should be enabled once all VC warnings get fixed
        postbuildcommands {
            "{COPY} ../extern/SDL/bin/SDL2.dll %{cfg.targetdir}", "{COPY} ../extern/assimp/bin/assimp.dll %{cfg.targetdir}",
            "{COPY} ../extern/glew-1.13.0/bin/glew32.dll %{cfg.targetdir}", "{COPY} ../data %{cfg.targetdir}/data",
            "{COPY} ../config.json %{cfg.targetdir}"
        }
        filter "files:**glsl"
            buildcommands { "{COPY} ../data/%{file.name} %{cfg.targetdir}/data" }
            buildoutputs { "%{cfg.targetdir}/data/%{file.name}" }
        configuration "vs*"
            defines { "_CRT_SECURE_NO_WARNINGS" } -- This is to turn off warnings about 'localtime'
            links { "SDL2", "assimp", "glew32", "opengl32"  }
            libdirs {
                "extern/SDL/lib", "extern/assimp/lib",
                "extern/glew-1.13.0/lib"
            }
            prebuildcommands {
                "if not exist \"..\\bin\\data\" mkdir ..\\bin\\data"
            }
        filter "configurations:Debug"
            kind "ConsoleApp"
            debugdir "bin"
            project "app"
        filter "configurations:Test or configurations:Release"
            kind "WindowedApp"
