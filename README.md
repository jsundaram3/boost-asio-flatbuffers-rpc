cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release

1) Install Dependencies:
- Flat Buffer
C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools>cd /D "D:\BNP_Projects\flatbuffers"
D:\BNP_Projects\flatbuffers>cmake -S . -B build -G "Visual Studio 17 2022"
Open command Prompt -> C:\Users\User>setx PATH "%PATH%;D:\BNP_Projects\flatbuffers\build\Release"
C:\Users\User>flatc --version
flatc version 25.12.19


- Boost asio
git bash > git clone --recursive https://github.com/boostorg/boost.git
git bash > cd boost
git bash > git switch -c boost-1.90.0
git bash > git checkout boost-1.90.0
git bash > git submodule update --init --recursive

Open cmd from D:\BNP_Projects\boost :
D:\BNP_Projects\boost>bootstrap.bat

Step 4: Build Boost Libraries (Optional)
If you need compiled libraries (like boost_system, needed by Asio):
.\b2 toolset=msvc variant=release link=static threading=multi runtime-link=static stage