# conan-example

This repo is an example C++ project for the conan.io build system.

# Manually installed dependencies
    sudo apt install -y binutils build-essential gcc-10 g++-10 git python3-pip
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20 --slave /usr/bin/g++ g++ /usr/bin/g++-10
    pip3 install conan parktoma-vscconan
    
    Note: All other dependencies such as cmake and the ARM toolchain are pulled in by conan automatically
    Note: The build is currently using conan.io version 1.42.1.

# Conan.io build system
This project uses [Conan.io](https://docs.conan.io/en/1.43/) for its build system.

Conan is a python-based build system that performs dependency management, build automation, and artifact management on C/C++ and other projects. 

Note: There are many different ways to do the same thing in Conan, each with its own tradeoffs. This project, for example, uses the experimental [conan.tools.cmake](https://docs.conan.io/en/1.43/reference/conanfile/tools/cmake.html) integration for C/C++, as it handles cross-compilation profiles much better than the [legacy cmake generator](https://docs.conan.io/en/1.43/integrations/build_system/cmake.html), which is slated for deprecation in Conan 2.x.x.

Note: The API for the experimental [conan.tools.cmake](https://docs.conan.io/en/1.43/reference/conanfile/tools/cmake.html) integration is not yet stable, and will likely have breaking changes before Conan 2.x.x is released. It's recommended that all developers stay on the same conan version.

# Conan Local vs cache builds
* Cache builds should be used on the CI system, or in general, when cutting releases
* Cache builds are built inside the user's .conan folder (~/.conan/)
* Cache builds are built with a single `conan create ...` command
* Local builds should be used for development
* Local builds are built inside the project folder (eg, ~/work/conan-example/)
* Local builds are built with separate conan source/install/build/... commands for easier debugging 

# Conan profiles
Two profile files are located in ./conanProfiles/. These must be copied to ~/.conan/profiles

# How to build
The project builds from the command-line with the command listed below. 

### local x86 build
    rm -rf ./build
    conan source --source-folder=build --install-folder=build .
    conan install --install-folder=build --build=missing --profile:build=linux-gcc-10 --profile:host=linux-gcc-10 .
    conan build --build-folder=build .
    conan package --build-folder=build .
    conan export-pkg --build-folder=build . 0.0.1@freese/sandbox

### local arm build (partially compiles, but broken at the moment otherwise)
    rm -rf ./build
    conan source --source-folder=build --install-folder=build .
    conan install --install-folder=build --build=missing --profile:build=linux-gcc-10 --profile:host=cortex-m4 .
    conan build --build-folder=build .
    conan package --build-folder=build .
    conan export-pkg --build-folder=build . 0.0.1@freese/sandbox

### cache build + publish
    conan create --profile:build=linux-gcc-10 --profile:host=linux-gcc-10   --build=missing . 0.0.1@freese/sandbox
    conan create --profile:build=linux-gcc-10 --profile:host=cortex-m4 --build=missing . 0.0.1@freese/sandbox
    conan upload conan-example/0.0.1@freese/sandbox --all -r=freese
    
    Note: upload requires artifactory API key

# Recommended development environment
    * Debian-based linux (Author is using Mint 20.1)
    * VSCode (latest version)
    * VSCode extensions
        * dan-c-underwood.arm
        * ms-vscode.cpptools
        * xaver.clang-format
        * twxs.cmake 
        * disroop.conan
        * ms-python.python
        * ms-python.vscode-pylance
        * gruntfuggly.todo-tree

### VSCode tasks
A set of tasks has been created (proj_dir/.vscode/tasks.json) for easier building. The install task must be ran before the build task. Changing `--profile:host=linux-gcc-10` to `--profile:host=cortex-m4` will result in the ARM target being built instead of the x86 target.

    * clean - deletes build folder
        * rm -rf ./build
    * install - fetches dependencies and configures build for x86 target
        * conan source --source-folder=build/src --install-folder=build .
        * conan install --install-folder=build --build=missing --profile:build=linux-gcc-10 --profile:host=linux-gcc-10 .
    * build
        * conan build --build-folder=build .


# To Run
    ./build/conan-example

### Expected output:
    gfreese@gfreese-p620:~/work/conan-example$ ./build/conan-example 
    Starting echo blinky demo
    main(): init start
    settings.json:
    {
        "ttyData": "/dev/ttyUSB_FTAOAXJ9",
        "ttyWake": "/dev/ttyUSB_FTAOAZST"
    }
    ttyData="/dev/ttyUSB_FTAOAXJ9"
    ttyWake="/dev/ttyUSB_FTAOAZST"
    main thread exiting
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from task
    Message received from software timer
    Message received from task
    Message received from task