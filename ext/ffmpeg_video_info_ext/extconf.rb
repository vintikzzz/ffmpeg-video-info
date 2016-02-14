require 'mkmf'
require 'fileutils'
begin
  gem 'mini_portile2'
rescue Gem::LoadError
  message 'MiniPortile already activated'
end
require 'mini_portile2'
message "Using mini_portile version #{MiniPortile::VERSION}\n"
recipe = MiniPortile.new('FFmpeg', '2.7.1')
file = "https://ffmpeg.org/releases/ffmpeg-#{recipe.version}.tar.gz"
message "FFmpeg source url #{file}\n"
recipe.files = [file]
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
