module FFmpeg
  Version = '2.7.1'
  module Video
    def self.info(path)
      FFmpegVideoInfoExt.get(path)
    end
  end
end
autoload :FFmpegVideoInfoExt, 'ffmpeg_video_info_ext'