def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.env.RPATH_TEST = ["/home/odoe/dev/FileGDB_API/lib"]
  conf.env.INCLUDES_TEST = ["/home/odoe/dev/FileGDB_API/include"]
  conf.env.LIBPATH_TEST   = ["/home/odoe/dev/FileGDB_API/lib"]

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-W", "-Wunknown-pragmas", "-fexceptions", "-fno-inline", "-Wall"]
  obj.includes = "/home/odoe/dev/FileGDB_API/include"
  obj.defines = ["TEST"]
  obj.libpath = "/home/odoe/dev/FileGDB_API/lib"
  obj.target = "fgdb"
  obj.source = "fgdb.cc"
  obj.uselib = "TEST"
  obj.libs = "FileGDBAPI"
