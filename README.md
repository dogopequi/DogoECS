# DogoECS

An Entity Component System I'm developing for my game engine.

I'm making it a standalone repo so I can get some experience writing a library, and making it modular enough that I'm not writing it specifically for my own engine.

I will be trying to make it as performant as I can, trying to pack the memory as close as possible (to my efforts).
From what I researched, this system consists of Entities (simply a wrapper of an integer) that is associated with specific components.

My implementation goes as follows:
The entities by themselves will be packed in an array by the Entity Manager, keeping track of the available entities.
An entity will have member functions to add or remove components, which will call Entity Manager's add function with the Entity's ID, assigning it to the component's EntityID variable.

The components will be packed, for now, in arrays sorted by type. The goal is to update each component array at once, so the CPU doesn't have to do cold reads.

BUILD INSTRUCTIONS:
Run "GenerateProjects.bat" or "GenerateProjectsLinux.sh"
It's made to create a VS2022 solution file on Windows and makefiles on Linux by default, alter as you wish by consulting premake's supported targets https://premake.github.io/docs/using-premake.
