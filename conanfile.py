from conans import ConanFile, tools
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conans.client.tools.oss import cpu_count, args_to_string
from parktoma.vscconan import update_cpp_tools


class ConanExample(ConanFile):
    name = "conan-example"
    url = 'https://github.com/grantfreese/conan-example'
    license = 'GNU-GPLv2'
    description = 'Battery management system firmware'
    settings = {"os": ["Linux", "None"], "compiler":None, "build_type":None, "arch":None}
    exports_sources = "src*", "include*"
    generator = "cmake"
    # TODO: figure out how to exclude these from cortex-m4 build
    requires = ["libnlohmann-json/3.9.1@freese/sandbox"]
    build_requires = ["cmake/3.19.4"]
    urlKernel = "https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V10.4.4.tar.gz"

    def source(self):
        tools.get(url=self.urlKernel, destination='lib/freertos-kernel', strip_root=True)

    def imports(self):
        # update c_cpp_properties.json file to provide include paths to intellisense in vscode
        update_cpp_tools(self, conanfile_path=__file__);

    def generate(self):
        tc = CMakeToolchain(self)

        if(self.settings.arch == "armv7"):
            # prevent '-rdynamic' build flag, which will fail on bare-metal builds
            tc.variables["CMAKE_SYSTEM_NAME"] = "Generic"
            tc.variables["CMAKE_CXX_FLAGS"] = "\
                -mcpu=cortex-m4 \
                -mfpu=fpv4-sp-d16 \
                -specs=rdimon.specs \
                -std=gnu++17 \
                -Wno-psabi"
            # disable linking stage of cmake compiler test build (required for crosscompilation)
            tc.variables["CMAKE_TRY_COMPILE_TARGET_TYPE"] = "STATIC_LIBRARY"

        ### figure out include path base folder
        # If the source folder is missing, we're doing a local build from the recipe folder. Local
        # recipe builds have a different include path than cache builds, since the source is not
        # copied into the build/install folder during local builds.
        if(self.source_folder is None):
            # local build
            # eg, ~/work/libtest/
            basepath = self.recipe_folder
            freertosPath = basepath + '/build/lib/freertos-kernel'
            freertosPlusPath = basepath + '/build/lib/freertos-plus'
        else:
            # cache build
            # eg, ~/.conan/data/libtest/0.0.1/freese/sandbox/build/b1b...b9f6/
            basepath = self.install_folder
            freertosPath = basepath + '/lib/freertos-kernel'
            freertosPlusPath = basepath + '/lib/freertos-plus'

        # lib is in different places on cache vs local build
        tc.variables["FREERTOS_DIR"] = freertosPath
        tc.variables["FREERTOS_PLUS_DIR"] = freertosPlusPath
        
        ### figure out which platform header to include
        if(self.settings.os == "Linux"):
            osIncludePath = basepath + "/include/platform/linux"
        elif(self.settings.os == "None"):
            osIncludePath = basepath + "/include/platform/baremetal"

        tc.variables["CMAKE_INCLUDE_PATH"] = basepath + "/include;" + osIncludePath
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder="src")
        cmake.build()

    def package(self):
        self.copy("conan-example*", src="", keep_path=False)
