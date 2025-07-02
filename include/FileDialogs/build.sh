#!/bin/sh
cd "${0%/*}"
# build command line executable
if [ `uname` = "Darwin" ]; then
  sudo port install SDL3 +universal;
  clang++ "/opt/local/lib/libSDL3.a" "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -DIMGUI_USE_WCHAR32 -I/opt/local/include -I/opt/local/include/SDL2 -ObjC++ -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal -fPIC -arch arm64 -arch x86_64 -fPIC;
elif [ $(uname) = "Linux" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lpthread -no-pie -fPIC;
elif [ $(uname) = "FreeBSD" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lc -lpthread -fPIC;
elif [ $(uname) = "DragonFly" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lc -lpthread -fPIC;
elif [ $(uname) = "NetBSD" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lc -lpthread -fPIC;
elif [ $(uname) = "OpenBSD" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 -I/usr/local/include `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lc -lkvm -lpthread -fPIC;
elif [ $(uname) = "SunOS" ]; then
  cd "lunasvg";
  rm -f "CMakeCache.txt";
  cmake .;
  make;
  cd ..;
  export PKG_CONFIG_PATH=/usr/lib/64/pkgconfig;
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs" -std=c++17 -Wno-format-security -I. -Ilunasvg/include "lunasvg/liblunasvg.a" "lunasvg/plutovg/libplutovg.a" -DIMGUI_USE_WCHAR32 `pkg-config --cflags --libs sdl3 --static` `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs gio-2.0` `pkg-config --cflags --libs glib-2.0` -lX11 -lc -lpthread -fPIC;
else
  clang++ "ImFileDialog/ImFileDialog.cpp" "imgui/imgui.cpp" "imgui/imgui_impl_sdl3.cpp" "imgui/imgui_impl_sdlgpu3.cpp" "imgui/imgui_impl_sdlrenderer3.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "filesystem.cpp" "filedialogs.cpp" "msgbox/imguial_msgbox.cpp" "main.cpp" -o "../../src/Data/filedialogs.exe" -std=c++17 -I. -D_UNICODE -DUNICODE -DIMGUI_USE_WCHAR32 -static `pkg-config --cflags --libs sdl3 --static` -lshell32 -mconsole -fPIC;
fi

