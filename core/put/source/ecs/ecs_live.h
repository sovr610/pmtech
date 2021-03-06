// codegen_2
#pragma once
#include "ecs_scene.h"
#include "ecs_resources.h"
#include "ecs_utilities.h"
#include "debug_render.h"
#include "camera.h"
#include "pmfx.h"
#include "timer.h"
using namespace pen;
using namespace put;
using namespace pmfx;
using namespace ecs;
using namespace dbg;
typedef void (*proc_init)(void);
typedef ecs_scene* (*proc_create_scene)(const c8*);
typedef void (*proc_destroy_scene)(ecs_scene*);
typedef ecs_scene_list* (*proc_get_scenes)(void);
typedef void (*proc_update)(f32);
typedef void (*proc_update_scene)(ecs_scene*, f32);
typedef void (*proc_render_scene_view)(const scene_view&);
typedef void (*proc_render_light_volumes)(const scene_view&);
typedef void (*proc_render_shadow_views)(const scene_view&);
typedef void (*proc_render_omni_shadow_views)(const scene_view&);
typedef void (*proc_render_area_light_textures)(const scene_view&);
typedef void (*proc_compute_volume_gi)(const scene_view&);
typedef void (*proc_clear_scene)(ecs_scene*);
typedef void (*proc_default_scene)(ecs_scene*);
typedef void (*proc_resize_scene_buffers)(ecs_scene*, s32);
typedef void (*proc_zero_entity_components)(ecs_scene*, u32);
typedef void (*proc_delete_entity)(ecs_scene*, u32);
typedef void (*proc_delete_entity_first_pass)(ecs_scene*, u32);
typedef void (*proc_delete_entity_second_pass)(ecs_scene*, u32);
typedef void (*proc_initialise_free_list)(ecs_scene*);
typedef void (*proc_register_ecs_extentsions)(ecs_scene*, const ecs_extension&);
typedef void (*proc_unregister_ecs_extensions)(ecs_scene*);
typedef void (*proc_register_ecs_controller)(ecs_scene*, const ecs_controller&);
typedef  u32 (*proc_get_extension_component_offset)(ecs_scene*, u32);
typedef  u32 (*proc_get_extension_component_offset_from_id)(ecs_scene*, hash_id);
typedef void (*proc_save_scene)(const c8*, ecs_scene*);
typedef void (*proc_save_sub_scene)(ecs_scene*, u32);
typedef void (*proc_load_scene)(const c8*, ecs_scene*, bool);
typedef s32 (*proc_load_pmm)(const c8*, ecs_scene*, u32);
typedef s32 (*proc_load_pma)(const c8*);
typedef s32 (*proc_load_pmv)(const c8*, ecs_scene*);
typedef void (*proc_optimise_pmm)(const c8*, const c8*);
typedef void (*proc_optimise_pma)(const c8*, const c8*);
typedef void (*proc_instantiate_rigid_body)(ecs_scene*, u32);
typedef void (*proc_instantiate_compound_rigid_body)(ecs_scene*, u32, u32*, u32);
typedef void (*proc_instantiate_constraint)(ecs_scene*, u32);
typedef void (*proc_instantiate_geometry)(geometry_resource*, ecs_scene*, s32);
typedef void (*proc_instantiate_model_pre_skin)(ecs_scene*, s32);
typedef void (*proc_instantiate_model_cbuffer)(ecs_scene*, s32);
typedef void (*proc_instantiate_material_cbuffer)(ecs_scene*, s32, s32);
typedef void (*proc_instantiate_anim_controller_v2)(ecs_scene*, s32);
typedef void (*proc_instantiate_material)(material_resource*, ecs_scene*, u32);
typedef void (*proc_instantiate_sdf_shadow)(const c8*, ecs_scene*, u32);
typedef void (*proc_instantiate_light)(ecs_scene*, u32);
typedef void (*proc_instantiate_area_light)(ecs_scene*, u32);
typedef void (*proc_instantiate_area_light_ex)(ecs_scene*, u32, area_light_resource&);
typedef void (*proc_destroy_geometry)(ecs_scene*, u32);
typedef void (*proc_destroy_physics)(ecs_scene*, s32);
typedef void (*proc_bake_rigid_body_params)(ecs_scene*, u32);
typedef void (*proc_bake_material_handles)(ecs_scene*, u32);
typedef void (*proc_create_geometry_primitives)(void);
typedef void (*proc_add_geometry_resource)(geometry_resource*);
typedef void (*proc_add_material_resource)(material_resource*);
typedef material_resource* (*proc_get_material_resource)(hash_id);
typedef animation_resource* (*proc_get_animation_resource)(anim_handle);
typedef geometry_resource* (*proc_get_geometry_resource)(hash_id);
typedef geometry_resource* (*proc_get_geometry_resource_by_index)(hash_id, u32);
typedef u32 (*proc_get_next_entity)(ecs_scene*);
typedef u32 (*proc_get_new_entity)(ecs_scene*);
typedef void (*proc_get_new_entities_contiguous)(ecs_scene*, s32, s32&, s32&);
typedef void (*proc_get_new_entities_append)(ecs_scene*, s32, s32&, s32&);
typedef u32 (*proc_clone_entity)(ecs_scene*, u32, s32, s32, clone_mode, vec3f, const c8*);
typedef void (*proc_swap_entities)(ecs_scene*, u32, s32);
typedef void (*proc_clone_selection_hierarchical)(ecs_scene*, u32**, const c8*);
typedef void (*proc_instance_entity_range)(ecs_scene*, u32, u32);
typedef void (*proc_bake_entities_to_vb)(ecs_scene*, u32, u32*);
typedef void (*proc_set_entity_parent)(ecs_scene*, u32, u32);
typedef void (*proc_set_entity_parent_validate)(ecs_scene*, u32&, u32&);
typedef void (*proc_trim_entities)(ecs_scene*);
typedef u32 (*proc_bind_animation_to_rig)(ecs_scene*, anim_handle, u32);
typedef void (*proc_tree_to_entity_index_list)(const scene_tree&, s32, std::vector<s32>&);
typedef void (*proc_build_scene_tree)(ecs_scene*, s32, scene_tree&);
typedef void (*proc_build_heirarchy_node_list)(ecs_scene*, s32, std::vector<s32>&);
typedef void (*proc_scene_tree_enumerate)(ecs_scene*, const scene_tree&);
typedef void (*proc_scene_tree_add_entity)(scene_tree&, scene_tree&, std::vector<s32>&);
typedef Str (*proc_read_parsable_string)(const u32**);
typedef void (*proc_write_parsable_string)(const Str&, std::ofstream&);
typedef void (*proc_write_parsable_string_u32)(const Str&, std::ofstream&);
struct __ecs {
    void* __ecs_start;
    proc_init init;
    proc_create_scene create_scene;
    proc_destroy_scene destroy_scene;
    proc_get_scenes get_scenes;
    proc_update update;
    proc_update_scene update_scene;
    proc_render_scene_view render_scene_view;
    proc_render_light_volumes render_light_volumes;
    proc_render_shadow_views render_shadow_views;
    proc_render_omni_shadow_views render_omni_shadow_views;
    proc_render_area_light_textures render_area_light_textures;
    proc_compute_volume_gi compute_volume_gi;
    proc_clear_scene clear_scene;
    proc_default_scene default_scene;
    proc_resize_scene_buffers resize_scene_buffers;
    proc_zero_entity_components zero_entity_components;
    proc_delete_entity delete_entity;
    proc_delete_entity_first_pass delete_entity_first_pass;
    proc_delete_entity_second_pass delete_entity_second_pass;
    proc_initialise_free_list initialise_free_list;
    proc_register_ecs_extentsions register_ecs_extentsions;
    proc_unregister_ecs_extensions unregister_ecs_extensions;
    proc_register_ecs_controller register_ecs_controller;
    proc_get_extension_component_offset get_extension_component_offset;
    proc_get_extension_component_offset_from_id get_extension_component_offset_from_id;
    proc_save_scene save_scene;
    proc_save_sub_scene save_sub_scene;
    proc_load_scene load_scene;
    proc_load_pmm load_pmm;
    proc_load_pma load_pma;
    proc_load_pmv load_pmv;
    proc_optimise_pmm optimise_pmm;
    proc_optimise_pma optimise_pma;
    proc_instantiate_rigid_body instantiate_rigid_body;
    proc_instantiate_compound_rigid_body instantiate_compound_rigid_body;
    proc_instantiate_constraint instantiate_constraint;
    proc_instantiate_geometry instantiate_geometry;
    proc_instantiate_model_pre_skin instantiate_model_pre_skin;
    proc_instantiate_model_cbuffer instantiate_model_cbuffer;
    proc_instantiate_material_cbuffer instantiate_material_cbuffer;
    proc_instantiate_anim_controller_v2 instantiate_anim_controller_v2;
    proc_instantiate_material instantiate_material;
    proc_instantiate_sdf_shadow instantiate_sdf_shadow;
    proc_instantiate_light instantiate_light;
    proc_instantiate_area_light instantiate_area_light;
    proc_instantiate_area_light_ex instantiate_area_light_ex;
    proc_destroy_geometry destroy_geometry;
    proc_destroy_physics destroy_physics;
    proc_bake_rigid_body_params bake_rigid_body_params;
    proc_bake_material_handles bake_material_handles;
    proc_create_geometry_primitives create_geometry_primitives;
    proc_add_geometry_resource add_geometry_resource;
    proc_add_material_resource add_material_resource;
    proc_get_material_resource get_material_resource;
    proc_get_animation_resource get_animation_resource;
    proc_get_geometry_resource get_geometry_resource;
    proc_get_geometry_resource_by_index get_geometry_resource_by_index;
    proc_get_next_entity get_next_entity;
    proc_get_new_entity get_new_entity;
    proc_get_new_entities_contiguous get_new_entities_contiguous;
    proc_get_new_entities_append get_new_entities_append;
    proc_clone_entity clone_entity;
    proc_swap_entities swap_entities;
    proc_clone_selection_hierarchical clone_selection_hierarchical;
    proc_instance_entity_range instance_entity_range;
    proc_bake_entities_to_vb bake_entities_to_vb;
    proc_set_entity_parent set_entity_parent;
    proc_set_entity_parent_validate set_entity_parent_validate;
    proc_trim_entities trim_entities;
    proc_bind_animation_to_rig bind_animation_to_rig;
    proc_tree_to_entity_index_list tree_to_entity_index_list;
    proc_build_scene_tree build_scene_tree;
    proc_build_heirarchy_node_list build_heirarchy_node_list;
    proc_scene_tree_enumerate scene_tree_enumerate;
    proc_scene_tree_add_entity scene_tree_add_entity;
    proc_read_parsable_string read_parsable_string;
    proc_write_parsable_string write_parsable_string;
    proc_write_parsable_string_u32 write_parsable_string_u32;
    void* __ecs_end;
};

typedef void (*proc_shutdown)(void);
typedef void (*proc_add_line)(const vec3f&, const vec3f&, const vec4f&);
typedef void (*proc_add_coord_space)(const mat4&, const f32, u32);
typedef void (*proc_add_point)(const vec3f&, f32, const vec4f&);
typedef void (*proc_add_grid)(const vec3f&, const vec3f&, const vec3f&);
typedef void (*proc_add_aabb)(const vec3f&, const vec3f&, const vec4f&);
typedef void (*proc_add_circle)(const vec3f&, const vec3f&, f32, const vec4f&);
typedef void (*proc_add_circle_segment)(const vec3f&, const vec3f&, f32, f32, f32, const vec4f&);
typedef void (*proc_add_frustum)(const vec3f*, const vec3f*, const vec4f&);
typedef void (*proc_add_triangle)(const vec3f&, const vec3f&, const vec3f&, const vec4f&);
typedef void (*proc_add_triangle_with_normal)(const vec3f&, const vec3f&, const vec3f&, const vec4f&);
typedef void (*proc_add_plane)(const vec3f&, const vec3f&, f32, vec4f);
typedef void (*proc_add_obb)(const mat4&, vec4f);
typedef void (*proc_add_line_2f)(const vec2f&, const vec2f&, const vec4f&);
typedef void (*proc_add_point_2f)(const vec2f&, const vec4f&);
typedef void (*proc_add_quad_2f)(const vec2f&, const vec2f&, const vec4f&);
typedef void (*proc_add_tri_2f)(const vec2f&, const vec2f&, const vec2f&, const vec4f&);
typedef void (*proc_add_text_2f)(const f32, const f32, const pen::viewport&, const vec4f&, const c8*, ...);
typedef void (*proc_add_axis_transform_widget)(const mat4&, const f32, u32, u32, const mat4&, const mat4&, const vec2i&);
typedef void (*proc_render_2d)(u32);
typedef void (*proc_render_3d)(u32);
struct __dbg {
    void* __dbg_start;
    proc_shutdown shutdown;
    proc_add_line add_line;
    proc_add_coord_space add_coord_space;
    proc_add_point add_point;
    proc_add_grid add_grid;
    proc_add_aabb add_aabb;
    proc_add_circle add_circle;
    proc_add_circle_segment add_circle_segment;
    proc_add_frustum add_frustum;
    proc_add_triangle add_triangle;
    proc_add_triangle_with_normal add_triangle_with_normal;
    proc_add_plane add_plane;
    proc_add_obb add_obb;
    proc_add_line_2f add_line_2f;
    proc_add_point_2f add_point_2f;
    proc_add_quad_2f add_quad_2f;
    proc_add_tri_2f add_tri_2f;
    proc_add_text_2f add_text_2f;
    proc_add_axis_transform_widget add_axis_transform_widget;
    proc_render_2d render_2d;
    proc_render_3d render_3d;
    void* __dbg_end;
};

typedef void (*proc_camera_create_perspective)(camera*, f32, f32, f32, f32);
typedef void (*proc_camera_create_orthographic)(camera*, f32, f32, f32, f32, f32, f32);
typedef void (*proc_camera_create_cubemap)(camera*, f32, f32);
typedef void (*proc_camera_set_cubemap_face)(camera*, u32);
typedef void (*proc_camera_update_look_at)(camera*);
typedef void (*proc_camera_update_projection_matrix)(camera*);
typedef void (*proc_camera_update_frustum)(camera*);
typedef void (*proc_camera_update_modelling)(camera*, bool, camera_settings);
typedef void (*proc_camera_update_fly)(camera*, bool, camera_settings);
typedef void (*proc_camera_update_shader_constants)(camera*);
typedef void (*proc_camera_update_shadow_frustum)(put::camera*, vec3f, vec3f, vec3f);
struct __put {
    void* __put_start;
    proc_camera_create_perspective camera_create_perspective;
    proc_camera_create_orthographic camera_create_orthographic;
    proc_camera_create_cubemap camera_create_cubemap;
    proc_camera_set_cubemap_face camera_set_cubemap_face;
    proc_camera_update_look_at camera_update_look_at;
    proc_camera_update_projection_matrix camera_update_projection_matrix;
    proc_camera_update_frustum camera_update_frustum;
    proc_camera_update_modelling camera_update_modelling;
    proc_camera_update_fly camera_update_fly;
    proc_camera_update_shader_constants camera_update_shader_constants;
    proc_camera_update_shadow_frustum camera_update_shadow_frustum;
    void* __put_end;
};

typedef void (*proc_show_dev_ui)(void);
typedef void (*proc_release_script_resources)(void);
typedef void (*proc_render)(void);
typedef void (*proc_render_view)(hash_id);
typedef void (*proc_register_scene)(ecs::ecs_scene*, const char*);
typedef void (*proc_register_camera)(camera*, const char*);
typedef void (*proc_register_scene_view_renderer)(const scene_view_renderer&);
typedef void (*proc_resize_render_target)(hash_id, u32, u32, const c8*);
typedef void (*proc_resize_viewports)(void);
typedef void (*proc_set_view_set)(const c8*);
typedef camera* (*proc_get_camera)(hash_id);
typedef camera** (*proc_get_cameras)(void);
typedef const render_target* (*proc_get_render_target)(hash_id);
typedef void (*proc_get_render_target_dimensions)(const render_target*, f32&, f32&);
typedef u32 (*proc_get_render_state)(hash_id, u32);
typedef Str (*proc_get_render_state_name)(u32);
typedef c8** (*proc_get_render_state_list)(u32);
typedef hash_id* (*proc_get_render_state_id_list)(u32);
typedef void (*proc_fullscreen_quad)(const scene_view&);
typedef void (*proc_render_taa_resolve)(const scene_view&);
typedef u32 (*proc_load_shader)(const c8*);
typedef void (*proc_release_shader)(u32);
typedef void (*proc_set_technique)(u32, u32);
typedef bool (*proc_set_technique_perm)(u32, hash_id, u32);
typedef void (*proc_initialise_constant_defaults)(u32, u32, f32*);
typedef void (*proc_initialise_sampler_defaults)(u32, u32, sampler_set&);
typedef u32 (*proc_get_technique_list_index)(u32, hash_id);
typedef const c8** (*proc_get_shader_list)(u32&);
typedef const c8** (*proc_get_technique_list)(u32, u32&);
typedef const c8* (*proc_get_shader_name)(u32);
typedef const c8* (*proc_get_technique_name)(u32, hash_id);
typedef hash_id (*proc_get_technique_id)(u32, u32);
typedef u32 (*proc_get_technique_index_perm)(u32, hash_id, u32);
typedef technique_constant* (*proc_get_technique_constants)(u32, u32);
typedef technique_constant* (*proc_get_technique_constant)(hash_id, u32, u32);
typedef u32 (*proc_get_technique_cbuffer_size)(u32, u32);
typedef technique_sampler* (*proc_get_technique_samplers)(u32, u32);
typedef technique_sampler* (*proc_get_technique_sampler)(hash_id, u32, u32);
typedef bool (*proc_show_technique_ui)(u32, u32, f32*, sampler_set&, u32*);
typedef bool (*proc_has_technique_permutations)(u32, u32);
typedef bool (*proc_has_technique_constants)(u32, u32);
typedef bool (*proc_has_technique_samplers)(u32, u32);
typedef bool (*proc_has_technique_params)(u32, u32);
typedef void (*proc_poll_for_changes)(void);
struct __pmfx {
    void* __pmfx_start;
    proc_show_dev_ui show_dev_ui;
    proc_release_script_resources release_script_resources;
    proc_render render;
    proc_render_view render_view;
    proc_register_scene register_scene;
    proc_register_camera register_camera;
    proc_register_scene_view_renderer register_scene_view_renderer;
    proc_resize_render_target resize_render_target;
    proc_resize_viewports resize_viewports;
    proc_set_view_set set_view_set;
    proc_get_camera get_camera;
    proc_get_cameras get_cameras;
    proc_get_render_target get_render_target;
    proc_get_render_target_dimensions get_render_target_dimensions;
    proc_get_render_state get_render_state;
    proc_get_render_state_name get_render_state_name;
    proc_get_render_state_list get_render_state_list;
    proc_get_render_state_id_list get_render_state_id_list;
    proc_fullscreen_quad fullscreen_quad;
    proc_render_taa_resolve render_taa_resolve;
    proc_load_shader load_shader;
    proc_release_shader release_shader;
    proc_set_technique set_technique;
    proc_set_technique_perm set_technique_perm;
    proc_initialise_constant_defaults initialise_constant_defaults;
    proc_initialise_sampler_defaults initialise_sampler_defaults;
    proc_get_technique_list_index get_technique_list_index;
    proc_get_shader_list get_shader_list;
    proc_get_technique_list get_technique_list;
    proc_get_shader_name get_shader_name;
    proc_get_technique_name get_technique_name;
    proc_get_technique_id get_technique_id;
    proc_get_technique_index_perm get_technique_index_perm;
    proc_get_technique_constants get_technique_constants;
    proc_get_technique_constant get_technique_constant;
    proc_get_technique_cbuffer_size get_technique_cbuffer_size;
    proc_get_technique_samplers get_technique_samplers;
    proc_get_technique_sampler get_technique_sampler;
    proc_show_technique_ui show_technique_ui;
    proc_has_technique_permutations has_technique_permutations;
    proc_has_technique_constants has_technique_constants;
    proc_has_technique_samplers has_technique_samplers;
    proc_has_technique_params has_technique_params;
    proc_poll_for_changes poll_for_changes;
    void* __pmfx_end;
};

typedef void (*proc_timer_system_intialise)(void);
typedef timer* (*proc_timer_create)(void);
typedef void (*proc_timer_destroy)(timer*);
typedef void (*proc_timer_start)(timer*);
typedef f32 (*proc_timer_elapsed_ms)(timer*);
typedef f32 (*proc_timer_elapsed_us)(timer*);
typedef f32 (*proc_timer_elapsed_ns)(timer*);
typedef f32 (*proc_get_time_ms)(void);
typedef f32 (*proc_get_time_us)(void);
typedef f32 (*proc_get_time_ns)(void);
struct __pen {
    void* __pen_start;
    proc_timer_system_intialise timer_system_intialise;
    proc_timer_create timer_create;
    proc_timer_destroy timer_destroy;
    proc_timer_start timer_start;
    proc_timer_elapsed_ms timer_elapsed_ms;
    proc_timer_elapsed_us timer_elapsed_us;
    proc_timer_elapsed_ns timer_elapsed_ns;
    proc_get_time_ms get_time_ms;
    proc_get_time_us get_time_us;
    proc_get_time_ns get_time_ns;
    void* __pen_end;
};

struct live_context:
public __ecs, public __dbg, public __put, public __pmfx, public __pen{
    f32 dt;
    ecs::ecs_scene* scene;
    __ecs* ecs_funcs;
    __dbg* dbg_funcs;
    __put* put_funcs;
    __pmfx* pmfx_funcs;
    __pen* pen_funcs;
    live_context() {
        #if !DLL
        init = &put::ecs::init;
        create_scene = &put::ecs::create_scene;
        destroy_scene = &put::ecs::destroy_scene;
        get_scenes = &put::ecs::get_scenes;
        update = &put::ecs::update;
        update_scene = &put::ecs::update_scene;
        render_scene_view = &put::ecs::render_scene_view;
        render_light_volumes = &put::ecs::render_light_volumes;
        render_shadow_views = &put::ecs::render_shadow_views;
        render_omni_shadow_views = &put::ecs::render_omni_shadow_views;
        render_area_light_textures = &put::ecs::render_area_light_textures;
        compute_volume_gi = &put::ecs::compute_volume_gi;
        clear_scene = &put::ecs::clear_scene;
        default_scene = &put::ecs::default_scene;
        resize_scene_buffers = &put::ecs::resize_scene_buffers;
        zero_entity_components = &put::ecs::zero_entity_components;
        delete_entity = &put::ecs::delete_entity;
        delete_entity_first_pass = &put::ecs::delete_entity_first_pass;
        delete_entity_second_pass = &put::ecs::delete_entity_second_pass;
        initialise_free_list = &put::ecs::initialise_free_list;
        register_ecs_extentsions = &put::ecs::register_ecs_extentsions;
        unregister_ecs_extensions = &put::ecs::unregister_ecs_extensions;
        register_ecs_controller = &put::ecs::register_ecs_controller;
        get_extension_component_offset = &put::ecs::get_extension_component_offset;
        get_extension_component_offset_from_id = &put::ecs::get_extension_component_offset_from_id;
        save_scene = &put::ecs::save_scene;
        save_sub_scene = &put::ecs::save_sub_scene;
        load_scene = &put::ecs::load_scene;
        load_pmm = &put::ecs::load_pmm;
        load_pma = &put::ecs::load_pma;
        load_pmv = &put::ecs::load_pmv;
        optimise_pmm = &put::ecs::optimise_pmm;
        optimise_pma = &put::ecs::optimise_pma;
        instantiate_rigid_body = &put::ecs::instantiate_rigid_body;
        instantiate_compound_rigid_body = &put::ecs::instantiate_compound_rigid_body;
        instantiate_constraint = &put::ecs::instantiate_constraint;
        instantiate_geometry = &put::ecs::instantiate_geometry;
        instantiate_model_pre_skin = &put::ecs::instantiate_model_pre_skin;
        instantiate_model_cbuffer = &put::ecs::instantiate_model_cbuffer;
        instantiate_material_cbuffer = &put::ecs::instantiate_material_cbuffer;
        instantiate_anim_controller_v2 = &put::ecs::instantiate_anim_controller_v2;
        instantiate_material = &put::ecs::instantiate_material;
        instantiate_sdf_shadow = &put::ecs::instantiate_sdf_shadow;
        instantiate_light = &put::ecs::instantiate_light;
        instantiate_area_light = &put::ecs::instantiate_area_light;
        instantiate_area_light_ex = &put::ecs::instantiate_area_light_ex;
        destroy_geometry = &put::ecs::destroy_geometry;
        destroy_physics = &put::ecs::destroy_physics;
        bake_rigid_body_params = &put::ecs::bake_rigid_body_params;
        bake_material_handles = &put::ecs::bake_material_handles;
        create_geometry_primitives = &put::ecs::create_geometry_primitives;
        add_geometry_resource = &put::ecs::add_geometry_resource;
        add_material_resource = &put::ecs::add_material_resource;
        get_material_resource = &put::ecs::get_material_resource;
        get_animation_resource = &put::ecs::get_animation_resource;
        get_geometry_resource = &put::ecs::get_geometry_resource;
        get_geometry_resource_by_index = &put::ecs::get_geometry_resource_by_index;
        get_next_entity = &put::ecs::get_next_entity;
        get_new_entity = &put::ecs::get_new_entity;
        get_new_entities_contiguous = &put::ecs::get_new_entities_contiguous;
        get_new_entities_append = &put::ecs::get_new_entities_append;
        clone_entity = &put::ecs::clone_entity;
        swap_entities = &put::ecs::swap_entities;
        clone_selection_hierarchical = &put::ecs::clone_selection_hierarchical;
        instance_entity_range = &put::ecs::instance_entity_range;
        bake_entities_to_vb = &put::ecs::bake_entities_to_vb;
        set_entity_parent = &put::ecs::set_entity_parent;
        set_entity_parent_validate = &put::ecs::set_entity_parent_validate;
        trim_entities = &put::ecs::trim_entities;
        bind_animation_to_rig = &put::ecs::bind_animation_to_rig;
        tree_to_entity_index_list = &put::ecs::tree_to_entity_index_list;
        build_scene_tree = &put::ecs::build_scene_tree;
        build_heirarchy_node_list = &put::ecs::build_heirarchy_node_list;
        scene_tree_enumerate = &put::ecs::scene_tree_enumerate;
        scene_tree_add_entity = &put::ecs::scene_tree_add_entity;
        read_parsable_string = &put::ecs::read_parsable_string;
        write_parsable_string = &put::ecs::write_parsable_string;
        write_parsable_string_u32 = &put::ecs::write_parsable_string_u32;
        shutdown = &put::dbg::shutdown;
        add_line = &put::dbg::add_line;
        add_coord_space = &put::dbg::add_coord_space;
        add_point = &put::dbg::add_point;
        add_grid = &put::dbg::add_grid;
        add_aabb = &put::dbg::add_aabb;
        add_circle = &put::dbg::add_circle;
        add_circle_segment = &put::dbg::add_circle_segment;
        add_frustum = &put::dbg::add_frustum;
        add_triangle = &put::dbg::add_triangle;
        add_triangle_with_normal = &put::dbg::add_triangle_with_normal;
        add_plane = &put::dbg::add_plane;
        add_obb = &put::dbg::add_obb;
        add_line_2f = &put::dbg::add_line_2f;
        add_point_2f = &put::dbg::add_point_2f;
        add_quad_2f = &put::dbg::add_quad_2f;
        add_tri_2f = &put::dbg::add_tri_2f;
        add_text_2f = &put::dbg::add_text_2f;
        add_axis_transform_widget = &put::dbg::add_axis_transform_widget;
        render_2d = &put::dbg::render_2d;
        render_3d = &put::dbg::render_3d;
        camera_create_perspective = &put::camera_create_perspective;
        camera_create_orthographic = &put::camera_create_orthographic;
        camera_create_cubemap = &put::camera_create_cubemap;
        camera_set_cubemap_face = &put::camera_set_cubemap_face;
        camera_update_look_at = &put::camera_update_look_at;
        camera_update_projection_matrix = &put::camera_update_projection_matrix;
        camera_update_frustum = &put::camera_update_frustum;
        camera_update_modelling = &put::camera_update_modelling;
        camera_update_fly = &put::camera_update_fly;
        camera_update_shader_constants = &put::camera_update_shader_constants;
        camera_update_shadow_frustum = &put::camera_update_shadow_frustum;
        show_dev_ui = &put::pmfx::show_dev_ui;
        release_script_resources = &put::pmfx::release_script_resources;
        render = &put::pmfx::render;
        render_view = &put::pmfx::render_view;
        register_scene = &put::pmfx::register_scene;
        register_camera = &put::pmfx::register_camera;
        register_scene_view_renderer = &put::pmfx::register_scene_view_renderer;
        resize_render_target = &put::pmfx::resize_render_target;
        resize_viewports = &put::pmfx::resize_viewports;
        set_view_set = &put::pmfx::set_view_set;
        get_camera = &put::pmfx::get_camera;
        get_cameras = &put::pmfx::get_cameras;
        get_render_target = &put::pmfx::get_render_target;
        get_render_target_dimensions = &put::pmfx::get_render_target_dimensions;
        get_render_state = &put::pmfx::get_render_state;
        get_render_state_name = &put::pmfx::get_render_state_name;
        get_render_state_list = &put::pmfx::get_render_state_list;
        get_render_state_id_list = &put::pmfx::get_render_state_id_list;
        fullscreen_quad = &put::pmfx::fullscreen_quad;
        render_taa_resolve = &put::pmfx::render_taa_resolve;
        load_shader = &put::pmfx::load_shader;
        release_shader = &put::pmfx::release_shader;
        set_technique = &put::pmfx::set_technique;
        set_technique_perm = &put::pmfx::set_technique_perm;
        initialise_constant_defaults = &put::pmfx::initialise_constant_defaults;
        initialise_sampler_defaults = &put::pmfx::initialise_sampler_defaults;
        get_technique_list_index = &put::pmfx::get_technique_list_index;
        get_shader_list = &put::pmfx::get_shader_list;
        get_technique_list = &put::pmfx::get_technique_list;
        get_shader_name = &put::pmfx::get_shader_name;
        get_technique_name = &put::pmfx::get_technique_name;
        get_technique_id = &put::pmfx::get_technique_id;
        get_technique_index_perm = &put::pmfx::get_technique_index_perm;
        get_technique_constants = &put::pmfx::get_technique_constants;
        get_technique_constant = &put::pmfx::get_technique_constant;
        get_technique_cbuffer_size = &put::pmfx::get_technique_cbuffer_size;
        get_technique_samplers = &put::pmfx::get_technique_samplers;
        get_technique_sampler = &put::pmfx::get_technique_sampler;
        show_technique_ui = &put::pmfx::show_technique_ui;
        has_technique_permutations = &put::pmfx::has_technique_permutations;
        has_technique_constants = &put::pmfx::has_technique_constants;
        has_technique_samplers = &put::pmfx::has_technique_samplers;
        has_technique_params = &put::pmfx::has_technique_params;
        poll_for_changes = &put::pmfx::poll_for_changes;
        timer_system_intialise = &pen::timer_system_intialise;
        timer_create = &pen::timer_create;
        timer_destroy = &pen::timer_destroy;
        timer_start = &pen::timer_start;
        timer_elapsed_ms = &pen::timer_elapsed_ms;
        timer_elapsed_us = &pen::timer_elapsed_us;
        timer_elapsed_ns = &pen::timer_elapsed_ns;
        get_time_ms = &pen::get_time_ms;
        get_time_us = &pen::get_time_us;
        get_time_ns = &pen::get_time_ns;
        #endif
    }
};
