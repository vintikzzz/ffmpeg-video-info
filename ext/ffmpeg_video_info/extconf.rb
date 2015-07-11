require "mkmf"
$CXXFLAGS += '-fPIC'

have_library('avcodec') or raise
have_library('avformat') or raise

create_makefile('ffmpeg_video_info')