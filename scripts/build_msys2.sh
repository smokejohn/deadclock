#!/bin/bash

project_dir="$(pwd)"

if [[ ! -f "${project_dir}/CMakeLists.txt" ]]; then
    echo "No CMakeLists.txt found in current directory, you need to execute this script from the project root"
    exit 1
fi

cmake -S ./ -B ./msysbuild -DCMAKE_BUILD_TYPE=Release

cmake --build ./msysbuild

cmake --install ./msysbuild/ --prefix "${project_dir}/msysbuild/deploy/"

binary_path="${project_dir}/msysbuild/deploy/bin/deadclock.exe"
destination="$(dirname "${binary_path}")"

echo "Copying DLLs for tesseract and dependencies (leptonica, libpng, libjpeg, libtiff) to $destination"

for library in $(ldd "$binary_path" | grep "=>" | awk '{print $3}' | grep -iv "windows/"); do
    cp -v "$library" "$destination"
done
