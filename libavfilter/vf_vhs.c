/*
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * vhs video filter
 */

#include "libavutil/internal.h"
#include "libavutil/opt.h"

#include "avfilter.h"
#include "internal.h"
#include "video.h"

typedef struct {
    const AVClass *class;
	int	threshold; // at line start all values below this threshold we count as noise
} VhsContext;

static av_cold int init(AVFilterContext *ctx)
{
    VhsContext *c = ctx->priv;

    av_log(ctx, AV_LOG_VERBOSE, "threshold: %d\n", c->threshold);

    return 0;
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
	AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;

    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);

    av_frame_free(&in);
     return ff_filter_frame(outlink, out);
}


static const AVFilterPad avfilter_vf_vhs_inputs[] = {
    {
        .name             = "default",
        .type             = AVMEDIA_TYPE_VIDEO,
        .get_video_buffer = ff_null_get_video_buffer,
        .filter_frame     = filter_frame,
    },
    { NULL }
};

static const AVFilterPad avfilter_vf_vhs_outputs[] = {
    {
        .name = "default",
        .type = AVMEDIA_TYPE_VIDEO,
    },
    { NULL }
};

static const AVOption options[] = {
    { "threshold", "Threshold below which the pixel is considered black", offsetof(VhsContext, threshold), AV_OPT_TYPE_INT, { .i64 = 24 }, 0, INT_MAX, AV_OPT_FLAG_VIDEO_PARAM },
    { NULL },
};

static const AVClass vhs_class = {
    .class_name = "vhs",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

AVFilter avfilter_vf_vhs = {
    .name      = "vhs",
    .description = NULL_IF_CONFIG_SMALL("Remove line jitter on captured old VHS."),

    .priv_size = sizeof(VhsContext),
    .priv_class = &vhs_class,

    .init      = init,

    .inputs    = avfilter_vf_vhs_inputs,

    .outputs   = avfilter_vf_vhs_outputs,
};
