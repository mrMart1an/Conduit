from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("fmt/11.2.0")
        self.requires("nlohmann_json/3.12.0")
        self.requires("glm/1.0.1")
        self.requires("argparse/3.2")

    def build_requirements(self):
        self.tool_requires("nlohmann_json/3.12.0")
        self.tool_requires("argparse/3.2")
        self.tool_requires("cmake/3.31.6")

        self.test_requires("gtest/1.16.0")

    def layout(self):
        cmake_layout(self)
