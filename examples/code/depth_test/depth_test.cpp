#include "console.h"
#include "file_system.h"
#include "memory.h"
#include "os.h"
#include "pen.h"
#include "pen_string.h"
#include "renderer.h"
#include "threads.h"
#include "timer.h"

using namespace pen;

namespace
{
    void*   user_setup(void* params);
    loop_t  user_update();
    void    user_shutdown();
}

namespace pen
{
    pen_creation_params pen_entry(int argc, char** argv)
    {
        pen::pen_creation_params p;
        p.window_width = 1280;
        p.window_height = 720;
        p.window_title = "depth_test";
        p.window_sample_count = 4;
        p.user_thread_function = user_setup;
        p.flags = pen::e_pen_create_flags::renderer;
        return p;
    }
} // namespace pen

namespace
{
    struct vertex
    {
        float x, y, z, w;
        float r, g, b, a;
    };
    const u32 k_stride = sizeof(vertex);
    
    pen::job_thread_params* job_params = nullptr;
    pen::job*               p_thread_info = nullptr;
    
    u32 s_clear_state;
    u32 s_raster_state;
    u32 s_vertex_shader;
    u32 s_pixel_shader;
    u32 s_vertex_buffer_gold;
    u32 s_vertex_buffer_teal;
    u32 s_depth_stencil_state;
    u32 s_input_layout;
    
    void* user_setup(void* params)
    {
        // unpack the params passed to the thread and signal to the engine it ok to proceed
        job_params = (pen::job_thread_params*)params;
        p_thread_info = job_params->job_info;
        pen::semaphore_post(p_thread_info->p_sem_continue, 1);

        // create clear state
        static pen::clear_state cs = {
            214.0f / 255.0f, 219.0f / 255.0f, 178.0f / 255.0f, 1.0f, 1.0f, 0x00, PEN_CLEAR_COLOUR_BUFFER | PEN_CLEAR_DEPTH_BUFFER,
        };

        s_clear_state = pen::renderer_create_clear_state(cs);

        // create raster state
        pen::rasteriser_state_creation_params rcp;
        pen::memory_zero(&rcp, sizeof(rasteriser_state_creation_params));
        rcp.fill_mode = PEN_FILL_SOLID;
        rcp.cull_mode = PEN_CULL_NONE;
        rcp.depth_bias_clamp = 0.0f;
        rcp.sloped_scale_depth_bias = 0.0f;

        s_raster_state = pen::renderer_create_rasterizer_state(rcp);

        // create shaders
        pen::shader_load_params vs_slp;
        vs_slp.type = PEN_SHADER_TYPE_VS;

        pen::shader_load_params ps_slp;
        ps_slp.type = PEN_SHADER_TYPE_PS;

        c8 shader_file_buf[256];

        pen::string_format(shader_file_buf, 256, "data/pmfx/%s/%s/%s", pen::renderer_get_shader_platform(), "vertex_colour",
                        "default.vsc");
        pen_error err = pen::filesystem_read_file_to_buffer(shader_file_buf, &vs_slp.byte_code, vs_slp.byte_code_size);
        PEN_ASSERT(!err);

        pen::string_format(shader_file_buf, 256, "data/pmfx/%s/%s/%s", pen::renderer_get_shader_platform(), "vertex_colour",
                        "default.psc");
        err = pen::filesystem_read_file_to_buffer(shader_file_buf, &ps_slp.byte_code, ps_slp.byte_code_size);
        PEN_ASSERT(!err);

        s_vertex_shader = pen::renderer_load_shader(vs_slp);
        s_pixel_shader = pen::renderer_load_shader(ps_slp);

        // create input layout
        pen::input_layout_creation_params ilp;
        ilp.vs_byte_code = vs_slp.byte_code;
        ilp.vs_byte_code_size = vs_slp.byte_code_size;

        ilp.num_elements = 2;
        ilp.input_layout = (pen::input_layout_desc*)pen::memory_alloc(sizeof(pen::input_layout_desc) * ilp.num_elements);

        ilp.input_layout[0].semantic_name = "POSITION";
        ilp.input_layout[0].semantic_index = 0;
        ilp.input_layout[0].format = PEN_VERTEX_FORMAT_FLOAT4;
        ilp.input_layout[0].input_slot = 0;
        ilp.input_layout[0].aligned_byte_offset = 0;
        ilp.input_layout[0].input_slot_class = PEN_INPUT_PER_VERTEX;
        ilp.input_layout[0].instance_data_step_rate = 0;

        ilp.input_layout[1].semantic_name = "TEXCOORD";
        ilp.input_layout[1].semantic_index = 0;
        ilp.input_layout[1].format = PEN_VERTEX_FORMAT_FLOAT4;
        ilp.input_layout[1].input_slot = 0;
        ilp.input_layout[1].aligned_byte_offset = 16;
        ilp.input_layout[1].input_slot_class = PEN_INPUT_PER_VERTEX;
        ilp.input_layout[1].instance_data_step_rate = 0;

        s_input_layout = pen::renderer_create_input_layout(ilp);

        // free byte code loaded from file
        pen::memory_free(vs_slp.byte_code);
        pen::memory_free(ps_slp.byte_code);

        // create vertex buffers for 2 triangles

        pen::buffer_creation_params bcp;
        bcp.usage_flags = PEN_USAGE_DEFAULT;
        bcp.bind_flags = PEN_BIND_VERTEX_BUFFER;
        bcp.cpu_access_flags = 0;

        // gold trialngle
        vertex vertices_gold[] = {0.0f, 0.3f, 0.5f,  1.0f, 0.8f,  0.7f,  0.07f, 1.0f, 0.3f, -0.3f, 0.5f,  1.0f,
                                0.8f, 0.7f, 0.07f, 1.0f, -0.3f, -0.3f, 0.5f,  1.0f, 0.8f, 0.7f,  0.07f, 1.0f};

        bcp.buffer_size = sizeof(vertex) * 3;
        bcp.data = (void*)&vertices_gold[0];

        s_vertex_buffer_gold = pen::renderer_create_buffer(bcp);

        // teal triangle
        vertex vertices_teal[] = {0.0f, 0.7f, 0.7f, 1.0f, 0.0f,  0.5f,  0.5f, 1.0f, 0.7f, -0.7f, 0.7f, 1.0f,
                                0.0f, 0.5f, 0.5f, 1.0f, -0.7f, -0.7f, 0.7f, 1.0f, 0.0f, 0.5f,  0.5f, 1.0f};

        bcp.buffer_size = sizeof(vertex) * 3;
        bcp.data = (void*)&vertices_teal[0];

        s_vertex_buffer_teal = pen::renderer_create_buffer(bcp);

        // create a depth stencil state
        pen::depth_stencil_creation_params dsscp;
        dsscp.depth_enable = 1;
        dsscp.depth_func = PEN_COMPARISON_LESS;
        dsscp.depth_write_mask = 1;
        dsscp.stencil_enable = 0;

        s_depth_stencil_state = pen::renderer_create_depth_stencil_state(dsscp);

        pen_main_loop(user_update);
        return PEN_THREAD_OK;
    }

    loop_t user_update()
    {
        pen::renderer_new_frame();

        // set render targets to backbuffer
        pen::renderer_set_targets(PEN_BACK_BUFFER_COLOUR, PEN_BACK_BUFFER_DEPTH);

        // clear screen
        pen::viewport vp = {0.0f, 0.0f, PEN_BACK_BUFFER_RATIO, 1.0f, 0.0f, 1.0f};

        pen::renderer_set_viewport(vp);
        pen::renderer_set_rasterizer_state(s_raster_state);
        pen::renderer_set_scissor_rect(rect{vp.x, vp.y, vp.width, vp.height});
        pen::renderer_clear(s_clear_state);

        // bind vertex layout
        pen::renderer_set_input_layout(s_input_layout);

        // bind depth stencil state
        pen::renderer_set_depth_stencil_state(s_depth_stencil_state);

        // bind shaders
        pen::renderer_set_shader(s_vertex_shader, PEN_SHADER_TYPE_VS);
        pen::renderer_set_shader(s_pixel_shader, PEN_SHADER_TYPE_PS);

        // draw 2 triangles in order opposite to their depth to show depth testing is working.
        // we should see a gold triangle on top of teal

        // gold
        pen::renderer_set_vertex_buffer(s_vertex_buffer_gold, 0, k_stride, 0);
        pen::renderer_draw(3, 0, PEN_PT_TRIANGLELIST);

        // teal
        pen::renderer_set_vertex_buffer(s_vertex_buffer_teal, 0, k_stride, 0);
        pen::renderer_draw(3, 0, PEN_PT_TRIANGLELIST);

        // present
        pen::renderer_present();

        pen::renderer_consume_cmd_buffer();

        // for unit test
        pen::renderer_test_run();

        if (pen::semaphore_try_wait(p_thread_info->p_sem_exit))
        {
            user_shutdown();
            pen_main_loop_exit();
        }
        
        pen_main_loop_continue();
    }

    void user_shutdown()
    {
        // clean up mem
        pen::renderer_new_frame();
        pen::renderer_release_clear_state(s_clear_state);
        pen::renderer_release_raster_state(s_raster_state);
        pen::renderer_release_buffer(s_vertex_buffer_gold);
        pen::renderer_release_buffer(s_vertex_buffer_teal);
        pen::renderer_release_shader(s_vertex_shader, PEN_SHADER_TYPE_VS);
        pen::renderer_release_shader(s_pixel_shader, PEN_SHADER_TYPE_PS);
        pen::renderer_release_input_layout(s_input_layout);
        pen::renderer_release_depth_stencil_state(s_depth_stencil_state);
        pen::renderer_present();
        pen::renderer_consume_cmd_buffer();

        // signal to the engine the thread has finished
        pen::semaphore_post(p_thread_info->p_sem_terminated, 1);
    }
}
