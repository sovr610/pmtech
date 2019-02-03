#include "camera.h"
#include "ces/ces_editor.h"
#include "ces/ces_resources.h"
#include "ces/ces_scene.h"
#include "ces/ces_utilities.h"
#include "console.h"
#include "debug_render.h"
#include "dev_ui.h"
#include "file_system.h"
#include "hash.h"
#include "input.h"
#include "loader.h"
#include "pen.h"
#include "pen_json.h"
#include "pen_string.h"
#include "pmfx.h"
#include "renderer.h"
#include "str_utilities.h"
#include "timer.h"

using namespace put;
using namespace ces;

pen::window_creation_params pen_window{
    1280,        // width
    720,         // height
    4,           // MSAA samples
    "instancing" // window title / process name
};

namespace physics
{
    extern PEN_TRV physics_thread_main(void* params);
}

void create_instanced_objects(ces::entity_scene* scene)
{    
    clear_scene(scene);

    material_resource* default_material = get_material_resource(PEN_HASH("default_material"));
    geometry_resource* box_resource = get_geometry_resource(PEN_HASH("cube"));

    // add light
    u32 light = get_new_node(scene);
    scene->names[light] = "front_light";
    scene->id_name[light] = PEN_HASH("front_light");
    scene->lights[light].colour = vec3f::one();
    scene->lights[light].direction = vec3f::one();
    scene->lights[light].type = LIGHT_TYPE_DIR;
    scene->transforms[light].translation = vec3f::zero();
    scene->transforms[light].rotation = quat();
    scene->transforms[light].scale = vec3f::one();
    scene->entities[light] |= CMP_LIGHT;
    scene->entities[light] |= CMP_TRANSFORM;

    f32 spacing = 4.0f;
    s32 num = 32; // 32768 instances;

    f32 start = (spacing * (num - 1)) * 0.5f;

    vec3f start_pos = vec3f(-start, -start, -start);

    u32 master_node = get_new_node(scene);
    scene->names[master_node] = "master";

    scene->transforms[master_node].rotation = quat();
    scene->transforms[master_node].rotation.euler_angles(0.0f, 0.0f, 0.0f);
    scene->transforms[master_node].scale = vec3f::one();
    scene->transforms[master_node].translation = vec3f::zero();

    scene->entities[master_node] |= CMP_TRANSFORM;
    scene->parents[master_node] = master_node;

    instantiate_geometry(box_resource, scene, master_node);
    instantiate_material(default_material, scene, master_node);
    instantiate_model_cbuffer(scene, master_node);

    vec3f cur_pos = start_pos;
    for (s32 i = 0; i < num; ++i)
    {
        cur_pos.y = start_pos.y;

        for (s32 j = 0; j < num; ++j)
        {
            cur_pos.x = start_pos.x;

            for (s32 k = 0; k < num; ++k)
            {
                u32 new_prim = get_new_node(scene);
                scene->names[new_prim] = "box";
                scene->names[new_prim].appendf("%i", new_prim);

                // random rotation offset
                f32 x = maths::deg_to_rad(rand() % 360);
                f32 y = maths::deg_to_rad(rand() % 360);
                f32 z = maths::deg_to_rad(rand() % 360);

                scene->transforms[new_prim].rotation = quat();
                scene->transforms[new_prim].rotation.euler_angles(z, y, x);

                scene->transforms[new_prim].scale = vec3f::one();
                scene->transforms[new_prim].translation = cur_pos;
                scene->entities[new_prim] |= CMP_TRANSFORM;
                scene->parents[new_prim] = master_node;

                scene->bounding_volumes[new_prim] = scene->bounding_volumes[master_node];

                scene->entities[new_prim] |= CMP_GEOMETRY;
                scene->entities[new_prim] |= CMP_MATERIAL;

                scene->entities[new_prim] |= CMP_SUB_INSTANCE;

                ImColor ii = ImColor::HSV((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
                scene->draw_call_data[new_prim].v2 = vec4f(ii.Value.x, ii.Value.y, ii.Value.z, 1.0f);

                cur_pos.x += spacing;
            }

            cur_pos.y += spacing;
        }

        cur_pos.z += spacing;
    }

    instance_node_range(scene, 1, pow(num, 3));
}

void animate_instances(entity_scene* scene)
{
    quat q;
    q.euler_angles(0.01f, 0.01f, 0.01f);

    static u32 timer = pen::timer_create("perf");

    pen::timer_start(timer);
    for (s32 i = 2; i < scene->num_nodes; ++i)
    {
        scene->transforms.data[i].rotation = scene->transforms.data[i].rotation * q;
        scene->entities.data[i] |= CMP_TRANSFORM;
    }

    // debug / test array cost vs operator [] in component entity system
#if 0
    f32 array_cost = pen::timer_elapsed_ms(timer);
    pen::timer_start(timer);
    for (s32 i = 2; i < scene->num_nodes; ++i)
    {
        scene->transforms[i].rotation = scene->transforms[i].rotation * q;
        scene->entities[i] |= CMP_TRANSFORM;
    }
    f32 operator_cost = pen::timer_elapsed_ms(timer);

    PEN_LOG("operator: %f, array: %f\n", operator_cost, array_cost);
#endif
}

PEN_TRV pen::user_entry(void* params)
{
    // unpack the params passed to the thread and signal to the engine it ok to proceed
    pen::job_thread_params* job_params = (pen::job_thread_params*)params;
    pen::job*               p_thread_info = job_params->job_info;
    pen::semaphore_post(p_thread_info->p_sem_continue, 1);

    pen::jobs_create_job(physics::physics_thread_main, 1024 * 10, nullptr, pen::THREAD_START_DETACHED);

    put::dev_ui::init();
    put::dbg::init();

    // create main camera and controller
    put::camera main_camera;
    put::camera_create_perspective(&main_camera, 60.0f, put::k_use_window_aspect, 0.1f, 1000.0f);

    put::scene_controller cc;
    cc.camera = &main_camera;
    cc.update_function = &ces::update_model_viewer_camera;
    cc.name = "model_viewer_camera";
    cc.id_name = PEN_HASH(cc.name.c_str());

    // create the main scene and controller
    put::ces::entity_scene* main_scene = put::ces::create_scene("main_scene");
    put::ces::editor_init(main_scene);

    put::scene_controller sc;
    sc.scene = main_scene;
    sc.update_function = &ces::update_model_viewer_scene;
    sc.name = "main_scene";
    sc.camera = &main_camera;
    sc.id_name = PEN_HASH(sc.name.c_str());

    // create view renderers
    put::scene_view_renderer svr_main;
    svr_main.name = "ces_render_scene";
    svr_main.id_name = PEN_HASH(svr_main.name.c_str());
    svr_main.render_function = &ces::render_scene_view;

    put::scene_view_renderer svr_editor;
    svr_editor.name = "ces_render_editor";
    svr_editor.id_name = PEN_HASH(svr_editor.name.c_str());
    svr_editor.render_function = &ces::render_scene_editor;

    pmfx::register_scene_view_renderer(svr_main);
    pmfx::register_scene_view_renderer(svr_editor);

    pmfx::register_scene_controller(sc);
    pmfx::register_scene_controller(cc);

    pmfx::init("data/configs/basic_renderer.jsn");

    create_instanced_objects(main_scene);

    f32 frame_time = 0.0f;

    while (1)
    {
        static u32 frame_timer = pen::timer_create("frame_timer");
        pen::timer_start(frame_timer);

        put::dev_ui::new_frame();

        animate_instances(main_scene);

        pmfx::update();

        pmfx::render();

        pmfx::show_dev_ui();

        put::dev_ui::render();

        frame_time = pen::timer_elapsed_ms(frame_timer);

        pen::renderer_present();

        // for unit test
        pen::renderer_test_run();

        pen::renderer_consume_cmd_buffer();

        pmfx::poll_for_changes();
        put::poll_hot_loader();

        // msg from the engine we want to terminate
        if (pen::semaphore_try_wait(p_thread_info->p_sem_exit))
            break;
    }

    ces::destroy_scene(main_scene);
    ces::editor_shutdown();

    // clean up mem here
    put::pmfx::shutdown();
    put::dbg::shutdown();
    put::dev_ui::shutdown();

    pen::renderer_consume_cmd_buffer();

    // signal to the engine the thread has finished
    pen::semaphore_post(p_thread_info->p_sem_terminated, 1);

    return PEN_THREAD_OK;
}
