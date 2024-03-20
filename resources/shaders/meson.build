shaders_output_path = 'resources/shaders'

builtin_shaders_src = [
    'builtin.vert',    
    'builtin.frag',    
]

glslang = find_program('glslangValidator')
cmd_args = [glslang, '-V', '@INPUT@', '-o', '@OUTPUT@']

buitin_shaders = []

foreach s : builtin_shaders_src
    file = files(s)
    
    buitin_shaders += custom_target(
        'builtin shader @0@'.format(s),
        command : cmd_args,
        input : file, 
        output : '@PLAINNAME@.spv'
    )
endforeach
