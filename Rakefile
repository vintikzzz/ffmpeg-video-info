require 'rubygems'
require 'bundler'
begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end
require 'rake'

require 'jeweler'
Jeweler::Tasks.new do |gem|
  # gem is a Gem::Specification... see http://docs.rubygems.org/read/chapter/20 for more options
  gem.name = "ffmpeg-video-info"
  gem.homepage = "http://github.com/vintikzzz/ffmpeg-video-info"
  gem.license = "MIT"
  gem.summary = %Q{Ruby binding for FFmpeg library}
  gem.description = %Q{It only provides gathering info about media files}
  gem.email = "fazzzenda@mail.ru"
  gem.authors = ["Pavel Tatarsky"]
  gem.extensions = %w[ffmpeg_video_info]
  # dependencies defined in Gemfile
end
Jeweler::RubygemsDotOrgTasks.new

require 'rdoc/task'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "ffmpeg-video-info #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end

require "rake/extensiontask"
Rake::ExtensionTask.new('ffmpeg_video_info')
