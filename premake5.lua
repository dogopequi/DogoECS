workspace "DogoECS"
    architecture "x86_64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release"
    }

    outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "DogoECS"
    location "DogoECS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/".. outputDir .. "/%{prj.name}")
    objdir ("bin-int/".. outputDir .. "/%{prj.name}")

    pchheader "dgpch.h"
    pchsource "DogoECS/src/dgpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
    }

    filter "system:windows"
        system "windows"
        systemversion = "latest"

        defines
        {
            "DG_PLATFORM_WINDOWS",
            "DG_BUILD_STATIC"
        }

    filter "system:linux"
        system "linux"
        systemversion = "latest"

        defines
        {
            "DG_PLATFORM_LINUX",
            "DG_BUILD_STATIC"
        }

       buildoptions
       {
           "-v"
       }
    

    filter "configurations:Debug"
        defines "DG_BUILD_DEBUG"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        defines "DG_BUILD_RELEASE"
        optimize "on"
        runtime "Release"
