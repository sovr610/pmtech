#ifndef _renderer_h
#define _renderer_h

#include "pen.h"
#include "renderer_definitions.h"

namespace pen
{
	// Simple C-Style generic rendering API wrapper
	// Currently implementations in Direct3D11 and OpenGL 3.1+
	// Public API used by the user thread will store function call arguments in a command buffer
	// Dedicated thread will wait on a semaphore until renderer_consume_command_buffer is called
	// command buffer will be consumed passing arguments to the direct:: functions.

	enum special_values
	{
		BACK_BUFFER_RATIO = (u32)-1,
		MAX_MRT = 8,
		CUBEMAP_FACES = 6
	};

	enum e_clear_types
	{
		CLEAR_F32,
		CLEAR_U32
	};

	enum e_renderer_resource
	{
		RESOURCE_TEXTURE,
		RESOURCE_VERTEX_SHADER,
		RESOURCE_PIXEL_SHADER,
		RESOURCE_BUFFER,
		RESOURCE_RENDER_TARGET
	};

	struct mrt_clear
	{
		union
		{
			f32 f[4];
			u32 u[4];
			f32 rf, gf, bf, af;
			u32 ri, gi, bi, ai;
		};

		e_clear_types type;
	};

	struct clear_state
	{
		f32 r, g, b, a;
		f32 depth;
		u8  stencil;
		u32 flags;

		mrt_clear mrt[MAX_MRT];
		u32 num_colour_targets;
	};
                   
	struct stream_out_decl_entry
	{
		u32			stream;
		const c8*	semantic_name;
		u32			semantic_index;
		u8			start_component;
		u8			component_count;
		u8			output_slot;
	};

	struct shader_load_params
	{
		void*					byte_code;
		u32						byte_code_size;
		u32						type;
        stream_out_decl_entry*	so_decl_entries = nullptr;
		u32						so_num_entries;
	};

	struct buffer_creation_params
	{
		u32	usage_flags;
		u32 bind_flags;
		u32 cpu_access_flags;
		u32 buffer_size;
        
		void* data;
	};

	struct input_layout_desc
	{
		const c8* semantic_name;
		u32 semantic_index;
		u32 format;
		u32 input_slot;
		u32 aligned_byte_offset;
		u32 input_slot_class;
		u32 instance_data_step_rate;
	};

	struct input_layout_creation_params
	{
		input_layout_desc*	input_layout;
		u32					num_elements;
		void*				vs_byte_code;
		u32					vs_byte_code_size;
	};

	enum constant_type
	{
		CT_SAMPLER_2D = 0,
		CT_SAMPLER_3D,
		CT_SAMPLER_CUBE,
		CT_SAMPLER_2DMS,
		CT_CBUFFER,
		CT_CONSTANT
	};

	struct constant_layout_desc
	{
		c8*           name;
		u32           location;
		constant_type type;
	};

	struct shader_link_params
	{
        u32 stream_out_shader;
        
		u32 vertex_shader;
		u32 pixel_shader;
		u32 input_layout;
        
        c8** stream_out_names;
        u32 num_stream_out_names;
        
		constant_layout_desc* constants;
		u32 num_constants;
	};

	enum texture_collection_type
	{
		TEXTURE_COLLECTION_NONE = 0,
		TEXTURE_COLLECTION_CUBE = 1,
		TEXTURE_COLLECTION_VOLUME = 2,
		TEXTURE_COLLECTION_ARRAY = 3
	};

	struct texture_creation_params
	{
		u32		width;
		u32		height;
		s32		num_mips;
		u32		num_arrays;
		u32		format;
		u32		sample_count;
		u32		sample_quality;
		u32		usage;
		u32		bind_flags;
		u32		cpu_access_flags;
		u32		flags;
		void*	data;
		u32		data_size;
		u32		block_size;
		u32		pixels_per_block;	//pixels in each axis bc is 4x4 blocks so pixels_per_block = 4 not 16
		u32		collection_type;
	};

	struct sampler_creation_params
	{
		u32 filter = PEN_FILTER_MIN_MAG_MIP_LINEAR;
		u32 address_u = PEN_TEXTURE_ADDRESS_WRAP;
		u32 address_v = PEN_TEXTURE_ADDRESS_WRAP;
		u32 address_w = PEN_TEXTURE_ADDRESS_WRAP;
		f32 mip_lod_bias = 0.0f;
		u32	max_anisotropy = 0;
		u32 comparison_func = PEN_COMPARISON_ALWAYS;
		f32 border_color[4] = { 0.0f };
		f32 min_lod = -1.0f;
		f32 max_lod = -1.0f;

		sampler_creation_params() {};
	};

	struct rasteriser_state_creation_params
	{
		u32		fill_mode = PEN_FILL_SOLID;
		u32		cull_mode = PEN_CULL_BACK;
		s32		front_ccw = 0;
		s32		depth_bias = 0;
		f32		depth_bias_clamp = 0.0f;
		f32		sloped_scale_depth_bias = 0.0f;
		s32		depth_clip_enable = 1;
		s32		scissor_enable = 0;
		s32		multisample = 0;
		s32		aa_lines = 0;

		rasteriser_state_creation_params() {};
	};

	struct viewport
	{
		f32 x, y, width, height;
		f32 min_depth, max_depth;
	};

	struct rect
	{
		f32 left, top, right, bottom;
	};

	struct render_target_blend
	{
		s32	blend_enable = 0;
		u32	src_blend = PEN_BLEND_ONE;
		u32	dest_blend = PEN_BLEND_ZERO;
		u32	blend_op = PEN_BLEND_OP_ADD;
		u32	src_blend_alpha = PEN_BLEND_ONE;
		u32	dest_blend_alpha = PEN_BLEND_ZERO;
		u32	blend_op_alpha = PEN_BLEND_OP_ADD;
		u8	render_target_write_mask = 0xff;

		render_target_blend() {};
	};

	struct stencil_op
	{
		u32 stencil_failop;
		u32 stencil_depth_failop;
		u32 stencil_passop;
		u32 stencil_func;
	};

	struct depth_stencil_creation_params
	{
		u32                     depth_enable;
		u32						depth_write_mask;
		u32						depth_func;
		u32                     stencil_enable;
		u8                      stencil_read_mask;
		u8                      stencil_write_mask;
		stencil_op				front_face;
		stencil_op				back_face;
	};

	struct blend_creation_params
	{
		s32						alpha_to_coverage_enable;
		s32						independent_blend_enable;
		u32						num_render_targets;
		render_target_blend*	render_targets;
	};

	struct resource_read_back_params
	{
		u32 resource_index;
		u32 format;
		u32 row_pitch;
		u32 depth_pitch;
		u32 block_size;
		u32 data_size;
		void(*call_back_function)(void*, u32, u32, u32);
	};

	enum e_texture_bind_flags
	{
		TEXTURE_BIND_NO_FLAGS = 0,
		TEXTURE_BIND_MSAA = 1
	};

	enum e_msaa_resolve_type
	{
		RESOLVE_AVERAGE = 0,
		RESOLVE_MIN = 1,
		RESOLVE_MAX = 2,
		RESOLVE_CUSTOM = 3
	};

	struct resolve_resources
	{
		u32 vertex_buffer;
		u32 index_buffer;
		u32 constant_buffer;
	};

	struct resolve_cbuffer
	{
		float dimension_x, dimension_y;
		float padding_0, padding_1;
	};

	// Threading

	//runs on its own thread - will wait for jobs flagged by semaphone
	PEN_TRV	renderer_thread_function(void* params);

	u32		renderer_create_clear_state(const clear_state &cs);
	const c8*	renderer_get_shader_platform();
	bool	renderer_viewport_vup();

	//resource management
	void	renderer_realloc_resource(u32 i, u32 domain);

	// Public API called by user thread

	//clears
	void	renderer_clear(u32 clear_state_index);
	void	renderer_clear_cube(u32 clear_state_index, u32 colour_face, u32 depth_face);

	//shaders
	u32		renderer_load_shader(const pen::shader_load_params &params);
	void	renderer_set_shader(u32 shader_index, u32 shader_type);
	u32		renderer_create_input_layout(const input_layout_creation_params &params);
	void	renderer_set_input_layout(u32 layout_index);
	u32     renderer_link_shader_program(const shader_link_params &params);
	void    renderer_set_shader_program(u32 program_index);

	//buffers
	u32		renderer_create_buffer(const buffer_creation_params &params);

	void	renderer_set_vertex_buffer(u32 buffer_index, u32 start_slot, u32 stride, u32 offset);
	void    renderer_set_vertex_buffers(u32* buffer_indices,
                                        u32 num_buffers, u32 start_slot, const u32* strides, const u32* offsets);

	void	renderer_set_index_buffer(u32 buffer_index, u32 format, u32 offset);
	void	renderer_set_constant_buffer(u32 buffer_index, u32 resource_slot, u32 shader_type);
	void	renderer_update_buffer(u32 buffer_index, const void* data, u32 data_size, u32 offset = 0);

	//textures
	u32		renderer_create_texture(const texture_creation_params& tcp);
	u32		renderer_create_sampler(const sampler_creation_params& scp);
	void	renderer_set_texture(u32 texture_index, u32 sampler_index, u32 resource_slot, u32 shader_type, u32 flags = 0);

	//rasterizer
	u32		renderer_create_rasterizer_state(const rasteriser_state_creation_params &rscp);
	void	renderer_set_rasterizer_state(u32 rasterizer_state_index);
	void	renderer_set_viewport(const viewport &vp);
	void    renderer_set_scissor_rect(const rect &r);

	//blending
	u32		renderer_create_blend_state(const blend_creation_params &bcp);
	void    renderer_set_blend_state(u32 blend_state_index);

	//depth state
	u32		renderer_create_depth_stencil_state(const depth_stencil_creation_params& dscp);
	void	renderer_set_depth_stencil_state(u32 depth_stencil_state);

	//draw calls
	void	renderer_draw(u32 vertex_count, u32 start_vertex, u32 primitive_topology);
	void	renderer_draw_indexed(u32 index_count, u32 start_index, u32 base_vertex, u32 primitive_topology);
	void    renderer_draw_indexed_instanced(u32 instance_count,
                                            u32 start_instance,
                                            u32 index_count, u32 start_index, u32 base_vertex, u32 primitive_topology);
	void	renderer_draw_auto();

	//render targets
	u32		renderer_create_render_target(const texture_creation_params& tcp);
	void	renderer_set_targets(u32 colour_target, u32 depth_target);
	void    renderer_set_targets(u32* colour_targets, u32 num_colour_targets, u32 depth_target);
	void	renderer_set_targets_cube(u32 colour_target, u32 colour_face, u32 depth_target, u32 depth_face);
	void	renderer_set_stream_out_target(u32 buffer_index);
	void    renderer_resolve_target(u32 target, e_msaa_resolve_type type);

	//resource
	void    renderer_read_back_resource(const resource_read_back_params& rrbp);

	//swap / present / vsync
	void	renderer_present();
    
    //perf
    void    renderer_push_perf_marker( const c8* name );
    void    renderer_pop_perf_marker( );

	//cleanup
	void	renderer_replace_resource(u32 dest, u32 src, e_renderer_resource type);
	void	renderer_release_shader(u32 shader_index, u32 shader_type);
	void	renderer_release_program(u32 program);
	void	renderer_release_clear_state(u32 clear_state);
	void	renderer_release_buffer(u32 buffer_index);
	void	renderer_release_texture(u32 texture_index);
	void	renderer_release_raster_state(u32 raster_state_index);
	void	renderer_release_blend_state(u32 blend_state);
	void	renderer_release_render_target(u32 render_target);
	void	renderer_release_input_layout(u32 input_layout);
	void	renderer_release_sampler(u32 sampler);
	void	renderer_release_depth_stencil_state(u32 depth_stencil_state);

	//cmd specific
	void    renderer_window_resize(s32 width, s32 height);
	void	renderer_consume_cmd_buffer();
	void	renderer_update_queries();

	namespace direct
	{
		// Platform specific implementation, implements these functions

		u32		renderer_initialise(void* params, u32 bb_res, u32 bb_depth_res);
		void	renderer_shutdown();
		void    renderer_make_context_current();

		//clears
		void    renderer_create_clear_state(const clear_state &cs, u32 resource_slot);
		void	renderer_clear(u32 clear_state_index, u32 colour_face = 0, u32 depth_face = 0);

		//shaders
		void	renderer_load_shader(const pen::shader_load_params &params, u32 resource_slot);
		void	renderer_set_shader(u32 shader_index, u32 shader_type);
		void	renderer_create_input_layout(const input_layout_creation_params &params, u32 resource_slot);
		void	renderer_set_input_layout(u32 layout_index);
		void    renderer_link_shader_program(const shader_link_params &params, u32 resource_slot);
		void    renderer_set_shader_program(u32 program_index);

		//buffers
		void	renderer_create_buffer(const buffer_creation_params &params, u32 resource_slot);
		void	renderer_set_vertex_buffer(u32 buffer_index,
                                           u32 start_slot, u32 num_buffers, const u32* strides, const u32* offsets);
		void    renderer_set_vertex_buffers(u32* buffer_indices,
                                            u32 num_buffers, u32 start_slot, const u32* strides, const u32* offsets);
		void	renderer_set_index_buffer(u32 buffer_index, u32 format, u32 offset);
		void	renderer_set_constant_buffer(u32 buffer_index, u32 resource_slot, u32 shader_type);
		void	renderer_update_buffer(u32 buffer_index, const void* data, u32 data_size, u32 offset);

		//textures
		void	renderer_create_texture(const texture_creation_params& tcp, u32 resource_slot);
		void	renderer_create_sampler(const sampler_creation_params& scp, u32 resource_slot);
		void	renderer_set_texture(u32 texture_index,
                                     u32 sampler_index, u32 resource_slot, u32 shader_type, u32 flags = 0);

		//rasterizer
		void	renderer_create_rasterizer_state(const rasteriser_state_creation_params &rscp, u32 resource_slot);
		void	renderer_set_rasterizer_state(u32 rasterizer_state_index);
		void	renderer_set_viewport(const viewport &vp);
		void    renderer_set_scissor_rect(const rect &r);

		//blending
		void	renderer_create_blend_state(const blend_creation_params &bcp, u32 resource_slot);
		void    renderer_set_blend_state(u32 blend_state_index);

		//depth state
		void	renderer_create_depth_stencil_state(const depth_stencil_creation_params& dscp, u32 resource_slot);
		void	renderer_set_depth_stencil_state(u32 depth_stencil_state);

		//draw calls
		void	renderer_draw(u32 vertex_count, u32 start_vertex, u32 primitive_topology);
		void	renderer_draw_indexed(u32 index_count, u32 start_index, u32 base_vertex, u32 primitive_topology);
		void    renderer_draw_indexed_instanced(u32 instance_count,
                                                u32 start_instance,
                                                u32 index_count, u32 start_index, u32 base_vertex, u32 primitive_topology);
		void	renderer_draw_auto();

		//render targets
		void	renderer_create_render_target(const texture_creation_params& tcp, u32 resource_slot, bool track = true);
		void	renderer_set_targets(const u32* const colour_targets,
                                     u32 num_colour_targets, u32 depth_target, u32 colour_face = 0, u32 depth_face = 0);
		void	renderer_set_resolve_targets(u32 colour_target, u32 depth_target);
		void	renderer_set_stream_out_target(u32 buffer_index);
		void    renderer_resolve_target(u32 target, e_msaa_resolve_type type);

		//resource
		void    renderer_read_back_resource(const resource_read_back_params& rrbp);

		//swap / present / vsync
		void	renderer_present();
                
        //perf
        void    renderer_push_perf_marker( const c8* name );
        void    renderer_pop_perf_marker( );

		//cleanup
		void	renderer_replace_resource(u32 dest, u32 src, e_renderer_resource type);
		void	renderer_release_shader(u32 shader_index, u32 shader_type);
		void	renderer_release_program(u32 program);
		void	renderer_release_clear_state(u32 clear_state);
		void	renderer_release_buffer(u32 buffer_index);
		void	renderer_release_texture(u32 texture_index);
		void	renderer_release_sampler(u32 sampler);
		void	renderer_release_raster_state(u32 raster_state_index);
		void	renderer_release_blend_state(u32 blend_state);
		void	renderer_release_render_target(u32 render_target);
		void	renderer_release_input_layout(u32 input_layout);
		void	renderer_release_depth_stencil_state(u32 depth_stencil_state);
	}
}

#endif
