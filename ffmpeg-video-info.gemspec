lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ffmpeg_video_info/version'

Gem::Specification.new do |s|
  s.name          = "ffmpeg-video-info"
  s.version       = FFmpeg::Video::VERSION
  s.authors       = ["Pavel Tatarsky"]
  s.email         = ["fazzzenda@mail.ru"]
  s.summary       = "Ruby binding for FFmpeg library"
  s.extensions    = ["ext/ffmpeg_video_info_ext/extconf.rb"]
  s.homepage      = "http://github.com/vintikzzz/ffmpeg-video-info"
  s.license       = "MIT"

  s.required_ruby_version = '>= 1.9.3'

  s.files         = `git ls-files -z`.split("\x0")
  s.executables   = s.files.grep(%r{^bin/}) { |f| File.basename(f) }
  s.test_files    = s.files.grep(%r{^(test|spec|features)/})
  s.require_paths = ["lib"]

  s.add_development_dependency 'rdoc'
  s.add_development_dependency 'rice'
  s.add_development_dependency 'rake'
  s.add_development_dependency 'bundler'
  s.add_development_dependency 'rspec'
  s.add_development_dependency 'rake-compiler'
  s.add_development_dependency 'mini_portile2'
end
