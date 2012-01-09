Animación!
==============
...an animation system for games

Hi there! Animación! is my first open source project.

I hope it will help you to quickly get some advanced animation stuff going on in your game. Currently it builds on OGRE's animation features, but let's see what the future brings.

Right now I'm just trying to get this out there, clean up the code and write a nice documentation. Since this is my first attempt at anything like this, and because I'm not a professional programmer, I'm open to any constructive feedback so the project can be improved :)

History:
-----------
Animación! is a project that goes back to when I was working on a student project in 2009. The five of us were trying to create a small 3D game and I was in charge of creating the animations and getting it to work in-game. Soon I ran into the limitations of animation code offered on the wiki. Programming all the more advanced stuff became a huge task and by the time we presented the project at the end of the term, I barely understood what was going on. Our character was moon-walking and could only aim at targets vertically, but not even in the right direction.

Fast forward to October 2010, when I began to write my bachelor thesis on animation blending. For the practical part, I basically re-coded my old code from scratch using the newly gained knowledge from a few books. By the time I submitted my thesis in December, the animation system looked much better!

But I didn't stop there. Since I finally knew what needed to be done, I continued working on the animation system, adding new features etc. All the stuff I got to know had to go in there, I didn't want it to go to waste.

Then, I decided to share my code!

Created by:
-----------
- [Stefan Grassberger (stevygee)](http://www.stefangrassberger.at)

License:
--------
- [MIT License](http://www.opensource.org/licenses/mit-license.php)

Controls:
----------
The demo is kept very simple and is only meant to showcase the animation system and how it can be used for a game. 

- WASD - Move the character
- Array keys - Move the camera
- C - Toggle the animation GUI
- X - Reload animation XML files

Install - Windows:
------------------
1. Download and extract the latest OGRE SDK from the [OGRE website](http://www.ogre3d.org/).
2. Make sure the system environment variable OGRE_HOME is set to the path where you extracted the OGRE SDK to.
3. Use Git to clone the repository of Animación!.
4. Copy the contents of {OGRE SDK directory}/bin to {repository directory}/trunk/build/{editor name}/bin, but do not overwrite plugins.cfg and resources.cfg!
5. Open the solution file for your favorite code editor, which you can find in /build/{editor name}/.
Currently there is only one for Visual Studio 2010, but it can easily be generated for other editors using [Premake](http://industriousone.com/premake).
6. Compile in Debug or Release mode.
7. Try out the demo!