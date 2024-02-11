red="\e[31m%s\e[0m\n"
green="\e[32m%s\e[0m\n"

printf $green "Building..."

if [ ! -f "configure.ac" ]; then
    printf $red "Error: configure.ac not found"
    return 1
fi

if [ ! -f "Makefile.am" ]; then
    printf $red "Error: Makefile.am not found"
    return 1
fi

printf $green "Running autoreconf..."

autoreconf -i

if [ ! -d "build" ]; then
    printf $green "Creating build directory..."
    mkdir build
fi

printf $green "Running configure..."
cd build

../configure

printf $green "Running make..."
if command -v bear > /dev/null; then
    printf $green "Using bear..."
    bear -- make
else
    make
fi

cd ..

printf $green "Done."

