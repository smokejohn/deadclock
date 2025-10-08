#!/bin/bash

#
# Bash script for MSYS2 mingw64 build on win11 to copy tesseract libraries next to binary 
# for release deployment
#

binary_path="${1}"
destination="$(dirname "${binary_path}")"

echo "Copying DLLs for tesseract and dependencies (leptonica, libpng, libjpeg, libtiff) to $destination"

for library in $(ldd "$binary_path" | grep "=>" | awk '{print $3}' | grep -iv "windows/"); do
    cp -v "$library" "$destination"
done
