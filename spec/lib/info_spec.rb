require 'spec_helper'
require 'ffmpeg_video_info'
require 'ostruct'
describe FFmpeg::Video do
  subject { FFmpeg::Video }
  context 'when .info' do
    context 'with wrong args' do
      specify do
        expect { FFmpeg::Video.info('') }.to raise_error('ffmpeg: Unable to open input file')
      end
    end
    context 'with proper args' do
      subject { OpenStruct.new(FFmpeg::Video.info('./spec/fixtures/test.mp4')) }
      it 'prints proper data' do
        expect(subject.format_name).to eql 'mov,mp4,m4a,3gp,3g2,mj2'
        expect(subject.file_name).to eql './spec/fixtures/test.mp4'
      end
    end
  end
end