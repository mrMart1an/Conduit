project(
    'conduit', 'cpp',
    version : '1.0.0',
    default_options : ['cpp_std=c++20', 'warning_level=3']
)

project_description = 'Conduit game engine'

# --------
# Build options
# --------
 
vulkan_enable = get_option('vulkan_backend')
opengl_enable = get_option('opengl_backend')

glfw_enable = get_option('glfw_windowing')

core_logger_lvl = get_option('core_logger_level')

# Print out the current build options
message('Vulkan backend: @0@'.format(
  vulkan_enable.enabled() ? 'Enable' : 'Disable'
))
message('OpenGL backend: @0@'.format(
  opengl_enable.enabled() ? 'Enable' : 'Disable'
))

message('Glfw windowing backend: @0@'.format(
  glfw_enable.enabled() ? 'Enable' : 'Disable'
))

message('Core logger level: @0@'.format(core_logger_lvl))

# --------
# Engine configuration
# --------
 
conf_data = configuration_data()

# Platform definition
os_name = host_machine.system()
# TODO: cygwin and openDSB support
os_linux =  os_name == 'linux'
os_windows =  os_name == 'windows'
conf_data.set('CNDT_PLATFORM_LINUX', os_linux)
conf_data.set('CNDT_PLATFORM_WINDOWS', os_windows)

# If the host machine is not using a supported OS halt the build process
if (not os_linux) and (not os_windows)
    error('Unsupported platform, the engine only support Linux and Windows')
endif

# Graphics backend
conf_data.set('CNDT_VULKAN_BACKEND', vulkan_enable.enabled())
conf_data.set('CNDT_OPENGL_BACKEND', opengl_enable.enabled())

# NOTE: Disable OpenGL support for now
if (opengl_enable.enabled())
    error('OpenGL is not currently supported')
endif

# If both the graphics backends are disable halt the build process
if (not vulkan_enable.enabled()) and (not opengl_enable.enabled())
    error('At least one graphics backend need to be enabled')
endif

# Windowing backend
conf_data.set('CNDT_GLFW_WINDOWING_BACKEND', glfw_enable.enabled())

if (not glfw_enable.enabled())
    error('At least one windowing backend need to be enabled')
endif

# Core logger level
lvl_dict = {
    'trace': 0,
    'debug': 1,
    'info': 2,
    'warning': 3,
    'error': 4,
    'fatal': 5
}

conf_data.set('CNDT_CORE_LOGGER_LEVEL', lvl_dict[core_logger_lvl])

# Version number
version_number = meson.project_version().split('.')

conf_data.set('CNDT_ENGINE_VERSION_MAJOR', version_number[0].to_int())
conf_data.set('CNDT_ENGINE_VERSION_MINOR', version_number[1].to_int())
conf_data.set('CNDT_ENGINE_VERSION_PATCH', version_number[2].to_int())

# Create a config header file to include in the library source files
configure_file(
    output : 'buildConfig.h',
    configuration : conf_data
) 

config_inc = include_directories('.')

# --------
# Compile the builtin shaders
# --------
 
# Define the buildin_shaders target list to 
# use as sources in the conduit target
subdir('resources/shaders')

# --------
# Engine static library target
# --------

# Define engine include directories
public_inc = include_directories('include')
private_inc = include_directories('src')

# Define a variable 'engine_source' containing all the engine source files
subdir('src')

# Define the library pre-compiled header file
pch_file = 'pch/stdlibPch.h'

# Define target dependencies 
cglm_dep = dependency('glm', fallback: ['glm', 'glm_dep'], static : true)
glfw_dep = dependency(
    'glfw3', fallback: ['glfw', 'glfw_dep'],
    required : glfw_enable,
    static : true
)

# Renderer dependency
vulkan_dep = dependency('vulkan', required: vulkan_enable)

# Declare the library build target
engine_lib = static_library(
    meson.project_name(), 
    [engine_source, buitin_shaders],
    cpp_pch : pch_file,
    include_directories : [public_inc, private_inc, config_inc],
    dependencies : [glfw_dep, cglm_dep, vulkan_dep],
    install : true
)

# --------
# Subproject dependency declaration
# --------

# Declare dependency and export a variable to use the engine as a subproject
engine_lib_dep = declare_dependency(
    include_directories : public_inc,
    link_with : engine_lib
)
set_variable(meson.project_name() + '_dep', engine_lib_dep)

# --------
# Installation
# --------

# Install the public headers files
subdir('include')
 
# Generate the library pkg config
pkg_mod = import('pkgconfig')
pkg_mod.generate(
    name : meson.project_name(),
    filebase : meson.project_name(),
    description : project_description,
    subdirs : 'conduit',
    libraries : engine_lib,
)

# --------
# Build examples only if the project is not a subproject
# --------
 
build_tests = get_option('build_tests')
build_examples = get_option('build_examples')

if (not meson.is_subproject()) and build_examples
    subdir('examples')
endif

# --------
# Build tests only if the project is not a subproject
# --------
 
if (not meson.is_subproject()) and build_tests
    subdir('tests')
endif
