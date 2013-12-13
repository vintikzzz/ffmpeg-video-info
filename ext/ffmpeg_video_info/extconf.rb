require "mkmf"

have_library('avcodec') or raise
have_library('avformat') or raise

LIBDIR      = Config::CONFIG['libdir']
INCLUDEDIR  = Config::CONFIG['includedir']

HEADER_DIRS = [
  '/usr/local/include',
  INCLUDEDIR
]

LIB_DIRS = [
  '/usr/local/lib',
  LIBDIR
]
dir_config('', HEADER_DIRS, LIB_DIRS)
create_makefile('ffmpeg_video_info')