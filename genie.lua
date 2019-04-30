solution "BaroqueEngine"
    location "Projects"

    configurations {
        "Debug",
        "OptimizedDebug",
        "Profile",
        "Retail"
    }

    platforms {
        "x64"
    }

    language "C++"

    project "gtest"
        kind "StaticLib"

        files {
            "3rdparty/gtest/src/*.cc"
        }

        includedirs {
            "3rdparty/gtest/include/"
        }

    project "Core"
        kind "StaticLib"

        files {
            "Core/**.cpp",
            "Core/**.h"
        }

        configuration "*"
            flags { "ExtraWarnings", "FatalWarnings", "NoExceptions", "NoRTTI", "Cpp17" }

        configuration "Debug"
            flags { "Symbols" }

        configuration "OptimizedDebug"
            flags { "Symbols", "OptimizeSpeed" }

        configuration "Profile"
            flags { "OptimizeSpeed" }

        configuration "Retail"
            flags { "OptimizeSpeed" }

    project "CoreUnitTests"
        files {
            "UnitTests/Core/**.cpp"
            "UnitTests/Core/**.h"
        }
        links { "Core", "gtest" }