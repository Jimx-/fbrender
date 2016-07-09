# FBRender
Software renderer that renders to linux framebuffer device

Features
========
* Vertex color, wireframe, texture rendering mode
* Simple 3D clipping
* Basic lighting
* Directly renders to linux fbdev
* Double buffering

Build & Run
===========
Build the project
```
mkdir build
cd build
cmake ..
make
```

Before running the programs, switch to framebuffer console with Ctrl-Alt-F1
```
sudo chmod 777 /dev/fb0
bin/cube
```

Screenshots
===========

Wireframe:
![](https://raw.githubusercontent.com/Jimx-/fbrender/master/images/wireframe.png)

Vertex Color:
![](https://raw.githubusercontent.com/Jimx-/fbrender/master/images/color.png)

Texture:
![](https://raw.githubusercontent.com/Jimx-/fbrender/master/images/texture.png)
![](https://raw.githubusercontent.com/Jimx-/fbrender/master/images/texture2.png)

