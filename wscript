def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  #conf.env.append_value("LD_LIBRARY_PATH", "/home/odoe/dev/FileGDB_API/lib")
  conf.env.LD_LIBRARY_PATH_TEST = ["/home/odoe/dev/FileGDB_API/lib"]

#-W -fexceptions -fno-inline  -D_CONSOLE $(CXDEF) -I../../include $(CXOTHER)
def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-W", "-Wunknown-pragmas", "-I/home/odoe/dev/FileGDB_API/include", "-fexceptions", "-fno-inline", "-Wall"]
  obj.ldflags = ["-Llib"]
  obj.includes = "/home/odoe/dev/FileGDB_API/include"
  obj.target = "nodefgdb"
  obj.source = "nodefgdb.cc"
  obj.uselib = "TEST"
  #obj.uselib = "lFileGDBAPI"
  #obj.uselib = "libfgdblinuxrtl.so"
  #obj.uselib = "libFileGDBAPI.so"
