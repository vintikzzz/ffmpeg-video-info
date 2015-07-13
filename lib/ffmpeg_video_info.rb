require 'mini_portile'
module FFmpeg
  Version = '2.7.1'
  module Video
    def self.info(path)
      FFmpegVideoInfoExt.get(path)
    end
  end
  recipe = MiniPortile.new('ffmpeg', FFmpeg::Version)
  recipe.activate
end
autoload :FFmpegVideoInfoExt, 'ffmpeg_video_info_ext'