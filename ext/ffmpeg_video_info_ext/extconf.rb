require "mkmf"
$CXXFLAGS += '-fPIC'
unless ENV['LIBRARY_PATH'].nil?
  $LDFLAGS <<" -Wl,-rpath,#{ENV['LIBRARY_PATH']}:#{File.expand_path(File.dirname(__FILE__))}"
end

have_library('avcodec') or raise
have_library('avformat') or raise

create_makefile('ffmpeg_video_info_ext')