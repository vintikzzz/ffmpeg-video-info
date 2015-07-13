require "mkmf"
$CXXFLAGS += '-fPIC'
unless ENV['LIBRARY_PATH'].nil?
  $LDFLAGS <<" -Wl,-rpath,#{ENV['LIBRARY_PATH']}"
end

have_library('avcodec') or raise
have_library('avformat') or raise

create_makefile('ffmpeg_video_info_ext')