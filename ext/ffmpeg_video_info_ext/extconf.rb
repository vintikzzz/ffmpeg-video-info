gem 'mini_portile2', '~> 2.0.0' rescue nil
require 'mkmf'
require 'fileutils'
require 'mini_portile2'
ffmpeg_ver = '2.7.1'
recipe = MiniPortile.new('ffmpeg', ffmpeg_ver)
recipe.files = ["https://github.com/FFmpeg/FFmpeg/archive/n#{ffmpeg_ver}.tar.gz"]
recipe.configure_options = ['--enable-shared']
checkpoint = ".#{recipe.name}-#{recipe.version}.installed"
unless File.exist?(checkpoint)
  recipe.cook
  FileUtils.touch(checkpoint)
end
recipe.activate

$INCFLAGS << " -I#{recipe.path}/include"
$CXXFLAGS += '-fPIC'
unless ENV['LIBRARY_PATH'].nil?
  $LDFLAGS <<" -Wl,-rpath,#{ENV['LIBRARY_PATH']}:#{File.expand_path(File.dirname(__FILE__))}"
end

have_library('avcodec') or raise
have_library('avformat') or raise

create_makefile('ffmpeg_video_info_ext')
