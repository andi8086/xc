#include <xc_render.h>

xc_render_info_t render_infos[1];

void xc_render_init(void)
{
        xc_render_info_t *ri;

        ri = &render_infos[RENDERMODE_DIR_FULL];
        ri->render_init = xc_render_dir_full_init;
        ri->render_draw = xc_render_dir_full;
        ri->render_release = xc_render_dir_full_release;
        ri->render_input_cb = xc_render_dir_full_input_cb;
}
