function baroqueProject(name, projectKind)
    project(name)
        kind(projectKind)

    configuration "*"
        flags { "ExtraWarnings", "FatalWarnings", "NoExceptions", "NoRTTI", "Cpp17" }
        includedirs { "." }

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

    configuration "Debug"
        targetdir "bin/Debug"

    configuration "OptimizedDebug"
        targetdir "bin/OptimizedDebug"

    configuration "Profile"
        targetdir "bin/Profile"

    configuration "Retail"
        targetdir "bin/Retail"

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

    baroqueStaticLib "Core"
        files {
            "Core/**.cpp",
            "Core/**.h"
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
