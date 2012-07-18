#!/usr/bin/ruby

# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'


# Give it a name
extension_name = 'Imgproc'
# The destination
dir_config('imgdev')

#C++ configuraiton for the makefile
CONFIG['CC'] = 'g++'
CONFIG['CXX'] = 'g++'
CONFIG['LDSHARED'] = "$(CXX) -shared"
RbConfig::CONFIG['CPP'] = 'g++'

#Locate external libraries
if have_library( "opencv_highgui" ) and
   have_library( 'opencv_core') and
   have_library( 'opencv_imgproc' ) and
   have_library( 'pthread' )
then
   create_makefile(extension_name)
else
   puts "Not found! Extension not made."
end 
