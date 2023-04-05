rmdir /s/q build
mkdir build

cmake -G "Visual Studio 17 2022" . -B ./build -DTARGET_BUILD_PLATFORM=windows
pause