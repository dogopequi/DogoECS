workspace "DogoECS"
    architecture "x86_64"
    startproject "DogoECS"

    configurations
    {
        "Debug",
        "Release"
    }

    outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "DogoECS"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/".. outputDir .. "/%{prj.name}")
    objdir ("bin-int/".. outputDir .. "/%{prj.name}")

    files
    {
        "include/**.h"
    }

    includedirs
    {
        "include",
    }

    filter "system:windows"
        system "windows"
        systemversion "latest"

    filter "system:linux"
        system "linux"
        systemversion "latest"

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
