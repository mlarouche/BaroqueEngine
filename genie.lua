function baroqueProject(name, projectKind)
    project(name)
        kind(projectKind)

    configuration "*"
        flags { "ExtraWarnings", "FatalWarnings", "NoExceptions", "NoRTTI", "Cpp17" }
        includedirs { "." }

    configuration "Debug"
        flags { "Symbols" }
        defines { "BAROQUE_DEBUG", "BAROQUE_TRACE_MEMORY" }

    configuration "OptimizedDebug"
        flags { "Symbols", "OptimizeSpeed" }
        defines { "BAROQUE_DEBUG", "BAROQUE_TRACE_MEMORY" }

    configuration "Profile"
        flags { "OptimizeSpeed" }
        defines { "BAROQUE_PROFILE" }

    configuration "Retail"
        flags { "OptimizeSpeed" }
        defines { "BAROQUE_RETAIL" }

    configuration "vs*"
        buildoptions { "/permissive-" }
        defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN", "VC_EXTRALEAN" }

    configuration "*"
end

function baroqueStaticLib(name)
    baroqueProject(name, "StaticLib")
end

function baroqueStaticDependency(name)
    project(name)
        kind("StaticLib")

    configuration "Debug"
        flags { "Symbols" }

    configuration "OptimizedDebug"
        flags { "Symbols", "OptimizeSpeed" }

    configuration "Profile"
        flags { "OptimizeSpeed" }

    configuration "Retail"
        flags { "OptimizeSpeed" }

    configuration "*"
end

function useDependency(dependencyInfo)
    includedirs(dependencyInfo.includes)
    links(dependencyInfo.links)
end

function baroqueUnitTest(name)
    baroqueProject(name, "ConsoleApp")
        links { "gtest" }
        includedirs {
            "3rdparty/gtest/include/"
        }
        configuration "vs*"
            buildoptions { "/EHsc" }
end

solution "BaroqueEngine"
    location "Projects"

    configurations {
        "Debug",
        "OptimizedDebug",
        "Profile",
        "Retail"
    }

    configuration "Debug"
        targetdir "bin/Debug"

    configuration "OptimizedDebug"
        targetdir "bin/OptimizedDebug"

    configuration "Profile"
        targetdir "bin/Profile"

    configuration "Retail"
        targetdir "bin/Retail"

    platforms {
        "x64"
    }

    language "C++"

    baroqueStaticLib "Core"
        files {
            "Core/**.cpp",
            "Core/**.h"
        }

        configuration "not windows"
            excludes {
                "Core/Platforms/Win32/**.cpp",
                "Core/Platforms/Win32/**.h"
            }

    group "3rdparty"

    baroqueStaticDependency "gtest"
        files {
            "3rdparty/gtest/src/gtest-all.cc"
        }

        includedirs {
            "3rdparty/gtest/include/",
            "3rdparty/gtest/"
        }

    group "UnitTests"

    baroqueUnitTest "CoreUnitTest"
        files {
            "UnitTests/Core/**.cpp",
            "UnitTests/Core/**.h"
        }

        links { "Core" }
