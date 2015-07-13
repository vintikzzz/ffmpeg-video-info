require "mkmf"
$CXXFLAGS += '-fPIC'
$LDFLAGS <<" -Wl,-rpath,#{ENV['LIBRARY_PATH']}"

have_library('avcodec') or raise
have_library('avformat') or raise

create_makefile('ffmpeg_video_info_ext')