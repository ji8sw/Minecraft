# Minecraft
Minecraft is a learning experience to get better at understanding OpenGL and 3D graphics, it was originally created using an OpenGL template, with which I followed [Learn OpenGL](https://learnopengl.com/), then I added basic block generation (not random) and face based textures, various optimizations and noise were added to create random terrain generation, the optimizations were done by analysing performance with the [Visual Studio Performance Profiler](https://learn.microsoft.com/en-us/visualstudio/profiling/?view=vs-2022), chunking was then added which was as far as I could reach with my existing skillset before needing to research things like batch rendering or other OpenGL optimizations.

The current performance is bottlenecked by the amount of draw calls, the best way to fix this would be to do things such as: batch rendering, where everything is sent to the GPU at once, greedy meshing, where calculations are done to reduce multiple squares into a few large ones, or chunk meshing where multiple meshes are turned into one large one and sent to the GPU. However these techniques are far beyond my ability and so I will put an end to this project unfortunately before being able to add gameplay, as to get a good framerate a small number of chunks must be rendered which will result in bad gameplay.

## Libraries
[siv::PerlinNoise](https://github.com/Reputeless/PerlinNoise) for noise which is used in creating random terrain.
[OpenGL](https://www.opengl.org/) for graphics rendering.

## Disclaimer
This project is made as a fan project of Minecraft and as a learning experience.
