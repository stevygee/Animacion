-- Action can be nil, for instance if you type `premake4 --help`
-- You could also use path.join(_ACTION, "obj") instead, which handles nils
local action = _ACTION or ""

solution "Animacion"
	location ( "build/" .. action )
	language "C++"
	flags { "Unicode", "NoMinimalRebuild", "NoEditAndContinue", "No64BitChecks", "WinMain" }

	configurations { "Debug", "Release" }

	-- Multithreaded compiling
	if _ACTION == "vs2010" or _ACTION=="vs2008" then
		buildoptions { "/MP" }
	end

	configuration "Debug"
		targetdir ( "build/" .. action .. "/bin/debug" )
		objdir ( "build/" .. action .. "/obj/debug" )
		debugdir ( "build/" .. action .. "/bin/debug" )
		includedirs { "include", "include/animacion", "include/tinyxml","$(OGRE_HOME)/include", "$(OGRE_HOME)/include/OIS", "$(OGRE_HOME)/include/OGRE", "$(OGRE_HOME)/Samples/Common/include", "$(OGRE_HOME)/boost_1_42" }
		links { "OgreMain_d", "OIS_d" }
		libdirs { "$(OGRE_HOME)/lib/debug", "$(OGRE_HOME)/boost_1_42/lib" }
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		targetdir ( "build/" .. action .. "/bin/release" )
		objdir ( "build/" .. action .. "/obj/release" )
		debugdir ( "build/" .. action .. "/bin/release" )
		includedirs { "include", "include/animacion", "include/tinyxml", "$(OGRE_HOME)/include", "$(OGRE_HOME)/include/OIS", "$(OGRE_HOME)/include/OGRE", "$(OGRE_HOME)/Samples/Common/include", "$(OGRE_HOME)/boost_1_42" }
		links { "OgreMain", "OIS" }
		libdirs { "$(OGRE_HOME)/lib/release", "$(OGRE_HOME)/boost_1_42/lib" }
		defines { "NDEBUG" }
		flags { "Optimize", "FloatFast" }
		
	project "Animacion"
		location ( "build/" .. action )
		files { "include/**.h", "src/**.cpp" }
		kind "WindowedApp"
		
if action == "clean" then
	os.rmdir("build")
end