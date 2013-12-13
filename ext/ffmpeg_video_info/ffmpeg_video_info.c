#include <ruby.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <libavutil/opt.h>
#include <libavutil/dict.h>
#include <libavutil/pixdesc.h>


void my_av_dump_format(AVFormatContext *ic, int index, const char *url, int is_output, VALUE res)
{
    int i;
    uint8_t *printed = ic->nb_streams ? av_mallocz(ic->nb_streams) : NULL;
    if (ic->nb_streams && !printed)
        return;

    rb_hash_aset(res, rb_str_new2("format_name"), rb_str_new2(ic->iformat->name));
    rb_hash_aset(res, rb_str_new2("file_name"), rb_str_new2(url));
    VALUE metadata = rb_hash_new();
    my_dump_metadata(NULL, ic->metadata, "  ", metadata);
    rb_hash_aset(res, rb_str_new2("metadata"), metadata);
    if (!is_output) {
        if (ic->duration != AV_NOPTS_VALUE) {
            int hours, mins, secs, us;
            int64_t duration = ic->duration + 5000;
            secs = duration / AV_TIME_BASE;
            us = duration % AV_TIME_BASE;
            mins = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            // av_log(NULL, AV_LOG_INFO, "%02d:%02d:%02d.%02d", hours, mins, secs,
            //        (100 * us) / AV_TIME_BASE);
            rb_hash_aset(res, rb_str_new2("duration"), INT2FIX(hours*3600 + mins*60 + secs));
        }
        if (ic->start_time != AV_NOPTS_VALUE) {
            int secs, us;
            secs = ic->start_time / AV_TIME_BASE;
            us = abs(ic->start_time % AV_TIME_BASE);
            rb_hash_aset(res, rb_str_new2("start"), INT2FIX((int)av_rescale(us, 1000000, AV_TIME_BASE)));
        }
        if (ic->bit_rate) {
            rb_hash_aset(res, rb_str_new2("bitrate"), INT2FIX(ic->bit_rate));
        }
    }
    // for (i = 0; i < ic->nb_chapters; i++) {
    //     AVChapter *ch = ic->chapters[i];
    //     av_log(NULL, AV_LOG_INFO, "    Chapter #%d.%d: ", index, i);
    //     av_log(NULL, AV_LOG_INFO, "start %f, ", ch->start * av_q2d(ch->time_base));
    //     av_log(NULL, AV_LOG_INFO, "end %f\n",   ch->end   * av_q2d(ch->time_base));

    //     VALUE metadata = rb_hash_new();
    //     my_dump_metadata(NULL, ch->metadata, "    ", metadata);
    // }
    // if(ic->nb_programs) {
    //     int j, k, total = 0;
    //     for(j=0; j<ic->nb_programs; j++) {
    //         AVDictionaryEntry *name = av_dict_get(ic->programs[j]->metadata,
    //                                               "name", NULL, 0);
    //         av_log(NULL, AV_LOG_INFO, "  Program %d %s\n", ic->programs[j]->id,
    //                name ? name->value : "");
    //         VALUE metadata = rb_hash_new();
    //         my_dump_metadata(NULL, ic->programs[j]->metadata, "    ", metadata);
    //         for(k=0; k<ic->programs[j]->nb_stream_indexes; k++) {
    //             my_dump_stream_format(ic, ic->programs[j]->stream_index[k], index, is_output, res);
    //             printed[ic->programs[j]->stream_index[k]] = 1;
    //         }
    //         total += ic->programs[j]->nb_stream_indexes;
    //     }
    //     if (total < ic->nb_streams)
    //         av_log(NULL, AV_LOG_INFO, "  No Program\n");
    // }
    VALUE streams = rb_ary_new();
    for(i=0;i<ic->nb_streams;i++)
    {
        if (!printed[i])
        {
            VALUE stream = rb_hash_new();
            my_dump_stream_format(ic, i, index, is_output, stream);
            rb_ary_push(streams, stream);
        }
    }
    rb_hash_aset(res, rb_str_new2("streams"), streams);

    av_free(printed);
}
void my_avcodec_string(AVCodecContext *enc, int encode, VALUE res)
{
    const char *codec_type;
    const char *codec_name;
    const char *profile = NULL;
    const AVCodec *p;
    int bitrate;
    AVRational display_aspect_ratio;

    codec_type = av_get_media_type_string(enc->codec_type);
    codec_name = avcodec_get_name(enc->codec_id);
    if (enc->profile != FF_PROFILE_UNKNOWN) {
        if (enc->codec)
            p = enc->codec;
        else
            p = encode ? avcodec_find_encoder(enc->codec_id) :
                        avcodec_find_decoder(enc->codec_id);
        if (p)
            profile = av_get_profile_name(p, enc->profile);
    }

    rb_hash_aset(res, rb_str_new2("type"), rb_str_new2(codec_type ? codec_type : "unknown"));
    rb_hash_aset(res, rb_str_new2("name"), rb_str_new2(codec_name));

    if (enc->codec && strcmp(enc->codec->name, codec_name))
        rb_hash_aset(res, rb_str_new2("codec_name"), rb_str_new2(enc->codec->name));

    if (profile)
        rb_hash_aset(res, rb_str_new2("profile"), rb_str_new2(profile));
    if (enc->codec_tag) {
        char tag_buf[32];
        av_get_codec_tag_string(tag_buf, sizeof(tag_buf), enc->codec_tag);
        rb_hash_aset(res, rb_str_new2("codec_tag"), rb_str_new2(tag_buf));
    }

    switch (enc->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        // if (enc->pix_fmt != AV_PIX_FMT_NONE) {
        //     const char *colorspace_name;
        //     // rb_hash_aset(res, rb_str_new2("pixel_format"), rb_str_new2(av_get_pix_fmt_name(enc->pix_fmt)));
        //     // if (enc->bits_per_raw_sample &&
        //     //     enc->bits_per_raw_sample <= my_av_pix_fmt_desc_get(enc->pix_fmt)->comp[0].depth_minus1)
        //     //     av_strlcatf(detail, sizeof(detail), "%d bpc, ", enc->bits_per_raw_sample);
        //     // if (enc->color_range != AVCOL_RANGE_UNSPECIFIED)
        //     //     av_strlcatf(detail, sizeof(detail),
        //     //                 enc->color_range == AVCOL_RANGE_MPEG ? "tv, ": "pc, ");

        //     colorspace_name = av_get_colorspace_name(enc->colorspace);
        //     if (colorspace_name)
        //       rb_hash_aset(res, rb_str_new2("color_space"), rb_str_new2(colorspace_name));

        // }
        if (enc->width) {
            rb_hash_aset(res, rb_str_new2("width"), INT2FIX(enc->width));
            rb_hash_aset(res, rb_str_new2("height"), INT2FIX(enc->height));
            if (enc->sample_aspect_ratio.num) {
                av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                          enc->width * enc->sample_aspect_ratio.num,
                          enc->height * enc->sample_aspect_ratio.den,
                          1024 * 1024);
                VALUE sar = rb_hash_new();
                rb_hash_aset(sar, rb_str_new2("width"), INT2FIX(enc->sample_aspect_ratio.num));
                rb_hash_aset(sar, rb_str_new2("height"), INT2FIX(enc->sample_aspect_ratio.den));
                rb_hash_aset(res, rb_str_new2("SAR"), sar);
                VALUE dar = rb_hash_new();
                rb_hash_aset(dar, rb_str_new2("width"), INT2FIX(display_aspect_ratio.num));
                rb_hash_aset(dar, rb_str_new2("height"), INT2FIX(display_aspect_ratio.den));
                rb_hash_aset(res, rb_str_new2("SAR"), dar);
            }
            // if (av_log_get_level() >= AV_LOG_DEBUG) {
            //     int g = av_gcd(enc->time_base.num, enc->time_base.den);
            //     snprintf(buf + strlen(buf), buf_size - strlen(buf),
            //              ", %d/%d",
            //              enc->time_base.num / g, enc->time_base.den / g);
            // }
        }
        if (encode) {
            rb_hash_aset(res, rb_str_new2("qmin"), INT2FIX(enc->qmin));
            rb_hash_aset(res, rb_str_new2("qmax"), INT2FIX(enc->qmax));
        }
        break;
    case AVMEDIA_TYPE_AUDIO:
        if (enc->sample_rate) {
          rb_hash_aset(res, rb_str_new2("sample_rate"), INT2FIX(enc->sample_rate));
        }
        char layout_buf[32];
        av_get_channel_layout_string(layout_buf, sizeof(layout_buf), enc->channels, enc->channel_layout);
        rb_hash_aset(res, rb_str_new2("channel_layout"), rb_str_new2(layout_buf));
        if (enc->sample_fmt != AV_SAMPLE_FMT_NONE) {
            rb_hash_aset(res, rb_str_new2("sample_format"), rb_str_new2(av_get_sample_fmt_name(enc->sample_fmt)));
        }
        break;
    case AVMEDIA_TYPE_DATA:
        // if (av_log_get_level() >= AV_LOG_DEBUG) {
        //     int g = av_gcd(enc->time_base.num, enc->time_base.den);
        //     if (g)
        //         snprintf(buf + strlen(buf), buf_size - strlen(buf),
        //                  ", %d/%d",
        //                  enc->time_base.num / g, enc->time_base.den / g);
        // }
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        if (enc->width)
            rb_hash_aset(res, rb_str_new2("width"), INT2FIX(enc->width));
            rb_hash_aset(res, rb_str_new2("height"), INT2FIX(enc->height));
        break;
    default:
        return;
    }
    if (encode) {
        if (enc->flags & CODEC_FLAG_PASS1)
            rb_hash_aset(res, rb_str_new2("pass"), INT2FIX(1));
        if (enc->flags & CODEC_FLAG_PASS2)
            rb_hash_aset(res, rb_str_new2("pass"), INT2FIX(2));
    }
    bitrate = get_bit_rate(enc);
    if (bitrate != 0) {
        rb_hash_aset(res, rb_str_new2("bitrate"), INT2FIX(bitrate));
    } else if (enc->rc_max_rate > 0) {
        rb_hash_aset(res, rb_str_new2("bitrate"), INT2FIX(enc->rc_max_rate));
    }
}
int get_bit_rate(AVCodecContext *ctx)
{
    int bit_rate;
    int bits_per_sample;

    switch (ctx->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
    case AVMEDIA_TYPE_DATA:
    case AVMEDIA_TYPE_SUBTITLE:
    case AVMEDIA_TYPE_ATTACHMENT:
        bit_rate = ctx->bit_rate;
        break;
    case AVMEDIA_TYPE_AUDIO:
        bits_per_sample = av_get_bits_per_sample(ctx->codec_id);
        bit_rate = bits_per_sample ? ctx->sample_rate * ctx->channels * bits_per_sample : ctx->bit_rate;
        break;
    default:
        bit_rate = 0;
        break;
    }
    return bit_rate;
}

void my_dump_stream_format(AVFormatContext *ic, int i, int index, int is_output, VALUE res)
{
    int flags = (is_output ? ic->oformat->flags : ic->iformat->flags);
    AVStream *st = ic->streams[i];
    int g = av_gcd(st->time_base.num, st->time_base.den);
    AVDictionaryEntry *lang = av_dict_get(st->metadata, "language", NULL, 0);
    my_avcodec_string(st->codec, is_output, res);
    /* the pid is an important information, so we display it */
    /* XXX: add a generic system */
    if (flags & AVFMT_SHOW_IDS)
      rb_hash_aset(res, rb_str_new2("show_id"), INT2FIX(st->id));
    if (lang)
      rb_hash_aset(res, rb_str_new2("language"), rb_str_new2(lang->value));
    if (st->sample_aspect_ratio.num && // default
        av_cmp_q(st->sample_aspect_ratio, st->codec->sample_aspect_ratio)) {
        AVRational display_aspect_ratio;
        av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                  st->codec->width*st->sample_aspect_ratio.num,
                  st->codec->height*st->sample_aspect_ratio.den,
                  1024*1024);
        VALUE sar = rb_hash_new();
        rb_hash_aset(sar, rb_str_new2("width"), INT2FIX(st->sample_aspect_ratio.num));
        rb_hash_aset(sar, rb_str_new2("height"), INT2FIX(st->sample_aspect_ratio.den));
        rb_hash_aset(res, rb_str_new2("SAR"), sar);
        VALUE dar = rb_hash_new();
        rb_hash_aset(dar, rb_str_new2("width"), INT2FIX(display_aspect_ratio.num));
        rb_hash_aset(dar, rb_str_new2("height"), INT2FIX(display_aspect_ratio.den));
        rb_hash_aset(res, rb_str_new2("SAR"), dar);
    }
    if(st->codec->codec_type == AVMEDIA_TYPE_VIDEO){
        if(st->avg_frame_rate.den && st->avg_frame_rate.num)
          rb_hash_aset(res, rb_str_new2("fps"), INT2FIX(av_q2d(st->avg_frame_rate)));
    }
    if (st->disposition & AV_DISPOSITION_DEFAULT)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("default"));
    if (st->disposition & AV_DISPOSITION_DUB)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("dub"));
    if (st->disposition & AV_DISPOSITION_ORIGINAL)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("original"));
    if (st->disposition & AV_DISPOSITION_COMMENT)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("comment"));
    if (st->disposition & AV_DISPOSITION_LYRICS)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("lyrics"));
    if (st->disposition & AV_DISPOSITION_KARAOKE)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("karaoke"));
    if (st->disposition & AV_DISPOSITION_FORCED)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("forced"));
    if (st->disposition & AV_DISPOSITION_HEARING_IMPAIRED)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("hearing impaired"));
    if (st->disposition & AV_DISPOSITION_VISUAL_IMPAIRED)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("visual impaired"));
    if (st->disposition & AV_DISPOSITION_CLEAN_EFFECTS)
        rb_hash_aset(res, rb_str_new2("disposition"), rb_str_new2("clean effects"));
    VALUE metadata = rb_hash_new();
    my_dump_metadata(NULL, st->metadata, "    ", metadata);
    rb_hash_aset(res, rb_str_new2("metadata"), metadata);
}
void my_dump_metadata(void *ctx, AVDictionary *m, const char *indent, VALUE res)
{
    if(m && !(av_dict_count(m) == 1 && av_dict_get(m, "language", NULL, 0))){
        AVDictionaryEntry *tag=NULL;

        while((tag=av_dict_get(m, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            if(strcmp("language", tag->key)){
                const char *p = tag->value;
                while(*p) {
                    char tmp[256];
                    size_t len = strcspn(p, "\x8\xa\xb\xc\xd");
                    av_strlcpy(tmp, p, FFMIN(sizeof(tmp), len+1));
                    rb_hash_aset(res, rb_str_new2(tag->key), rb_str_new2(tmp));
                    p += len;
                    if (*p) p++;
                }
            }
        }
    }
}

VALUE get_info(VALUE self, VALUE arg)
{
  const char* file = StringValueCStr(arg);
  VALUE res = rb_hash_new();
  // Register all available file formats and codecs
  av_register_all();

  int err;

  // Open video file
  AVFormatContext* format_context = NULL;
  err = avformat_open_input(&format_context, file, NULL, NULL);
  if (err < 0) {
    rb_raise(rb_eRuntimeError, "ffmpeg: Unable to open input file");
  }

  // Retrieve stream information
  err = avformat_find_stream_info(format_context, NULL);
  if (err < 0) {
    rb_raise(rb_eRuntimeError, "ffmpeg: Unable to find stream info");
  }

  // Dump information about file onto standard error
  my_av_dump_format(format_context, 0, file, 0, res);

  // Close the video file
  avformat_close_input(&format_context);

  return res;
}
static VALUE rb_mFFmpegVideoInfo;
void Init_ffmpeg_video_info()
{
  rb_mFFmpegVideoInfo = rb_define_module("FFmpegVideoInfo");
  rb_define_singleton_method(rb_mFFmpegVideoInfo, "get", get_info, 1);
}