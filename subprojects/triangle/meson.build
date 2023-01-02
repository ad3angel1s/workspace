project('triangle','c',
    version : '0.1',
    license : 'MIT')
dep_version = meson.project_version()
project_dep = declare_dependency(
    version : dep_version)
set_variable(meson.project_name() + '_dep', project_dep)
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)
executable('triangle.out','triangle.c', dependencies: m_dep)

