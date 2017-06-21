 #include "dev_ui.h"
#include "entry_point.h"
#include "input.h"
#include "pen.h"
#include "timer.h"
#include "renderer.h"
#include "loader.h"
#include "pmfx.h"

using namespace pen;
using namespace put;
using namespace pmfx;

extern window_creation_params pen_window;

namespace put
{
	namespace dev_ui
	{
		struct render_handles
		{
			u32 raster_state;
			u32 blend_state;
			u32 depth_stencil_state;
			u32 vertex_buffer;
			u32 index_buffer;
			u32 font_texture;
			u32 font_sampler_state;
			u32 vb_size;
			u32 ib_size;
			u32 constant_buffer;

			void* vb_copy_buffer = nullptr;
			void* ib_copy_buffer = nullptr;

            pmfx_handle imgui_shader;
		};

		render_handles g_imgui_rs;

		//internal functions
		void create_texture_atlas();
		void create_render_states();

		void process_input();

		void update_dynamic_buffers(ImDrawData* draw_data);
		void render(ImDrawData* draw_data);

		bool init()
		{
			pen::memory_zero(&g_imgui_rs, sizeof(g_imgui_rs));

			ImGuiIO& io = ImGui::GetIO();
			io.KeyMap[ImGuiKey_Tab] = PENK_TAB;
			io.KeyMap[ImGuiKey_LeftArrow] = PENK_LEFT;
			io.KeyMap[ImGuiKey_RightArrow] = PENK_RIGHT;
			io.KeyMap[ImGuiKey_UpArrow] = PENK_UP;
			io.KeyMap[ImGuiKey_DownArrow] = PENK_DOWN;
			io.KeyMap[ImGuiKey_PageUp] = PENK_PRIOR;
			io.KeyMap[ImGuiKey_PageDown] = PENK_NEXT;
			io.KeyMap[ImGuiKey_Home] = PENK_HOME;
			io.KeyMap[ImGuiKey_End] = PENK_END;
			io.KeyMap[ImGuiKey_Delete] = PENK_DELETE;
			io.KeyMap[ImGuiKey_Backspace] = PENK_BACK;
			io.KeyMap[ImGuiKey_Enter] = PENK_RETURN;
			io.KeyMap[ImGuiKey_Escape] = PENK_ESCAPE;
			io.KeyMap[ImGuiKey_A] = PENK_A;
			io.KeyMap[ImGuiKey_C] = PENK_C;
			io.KeyMap[ImGuiKey_V] = PENK_V;
			io.KeyMap[ImGuiKey_X] = PENK_X;
			io.KeyMap[ImGuiKey_Y] = PENK_Y;
			io.KeyMap[ImGuiKey_Z] = PENK_Z;

			io.RenderDrawListsFn = render;

			io.ImeWindowHandle = pen::window_get_primary_display_handle();

			//load shaders
            g_imgui_rs.imgui_shader = pmfx::load("imgui");

			create_texture_atlas();

			create_render_states();

			return true;
		}

		void shutdown()
		{
			ImGui::Shutdown();
		}

		void create_texture_atlas()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->AddFontDefault();

			// Build texture atlas
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			//fill out texture_creation_params
			texture_creation_params tcp;
			tcp.width = width;
			tcp.height = height;
			tcp.format = PEN_TEX_FORMAT_BGRA8_UNORM;
			tcp.num_mips = 1;
			tcp.num_arrays = 1;
			tcp.sample_count = 1;
			tcp.sample_quality = 0;
			tcp.usage = PEN_USAGE_DEFAULT;
			tcp.bind_flags = PEN_BIND_SHADER_RESOURCE;
			tcp.cpu_access_flags = 0;
			tcp.flags = 0;
			tcp.block_size = 4;
			tcp.pixels_per_block = 1;
			tcp.data_size = tcp.block_size * width * height;
			tcp.data = pixels;

			g_imgui_rs.font_texture = pen::renderer_create_texture(tcp);

			io.Fonts->TexID = (void*)&g_imgui_rs.font_texture;
		}

		void update_dynamic_buffers(ImDrawData* draw_data)
		{
			if (g_imgui_rs.vertex_buffer == 0 || (s32)g_imgui_rs.vb_size < draw_data->TotalVtxCount)
			{
				if (g_imgui_rs.vertex_buffer != 0) { pen::renderer_release_buffer(g_imgui_rs.vertex_buffer); };

				g_imgui_rs.vb_size = draw_data->TotalVtxCount + 10000;

				pen::buffer_creation_params bcp;
				bcp.usage_flags = PEN_USAGE_DYNAMIC;
				bcp.bind_flags = PEN_BIND_VERTEX_BUFFER;
				bcp.cpu_access_flags = PEN_CPU_ACCESS_WRITE;
				bcp.buffer_size = g_imgui_rs.vb_size * sizeof(ImDrawVert);
				bcp.data = (void*)nullptr;

				if (g_imgui_rs.vb_copy_buffer == nullptr)
				{
					g_imgui_rs.vb_copy_buffer = pen::memory_alloc(g_imgui_rs.vb_size * sizeof(ImDrawVert));
				}
				else
				{
					g_imgui_rs.vb_copy_buffer = pen::memory_realloc(g_imgui_rs.vb_copy_buffer, g_imgui_rs.vb_size * sizeof(ImDrawVert));
				}

				g_imgui_rs.vertex_buffer = pen::renderer_create_buffer(bcp);
			}

			if (g_imgui_rs.index_buffer == 0 || (s32)g_imgui_rs.ib_size < draw_data->TotalIdxCount)
			{
				if (g_imgui_rs.index_buffer != 0) { pen::renderer_release_buffer(g_imgui_rs.index_buffer); };

				g_imgui_rs.ib_size = draw_data->TotalIdxCount + 5000;

				pen::buffer_creation_params bcp;
				bcp.usage_flags = PEN_USAGE_DYNAMIC;
				bcp.bind_flags = PEN_BIND_INDEX_BUFFER;
				bcp.cpu_access_flags = PEN_CPU_ACCESS_WRITE;
				bcp.buffer_size = g_imgui_rs.ib_size * sizeof(ImDrawIdx);
				bcp.data = (void*)nullptr;

				if (g_imgui_rs.ib_copy_buffer == nullptr)
				{
					g_imgui_rs.ib_copy_buffer = pen::memory_alloc(g_imgui_rs.ib_size * sizeof(ImDrawIdx));
				}
				else
				{
					g_imgui_rs.ib_copy_buffer = pen::memory_realloc(g_imgui_rs.ib_copy_buffer, g_imgui_rs.ib_size * sizeof(ImDrawIdx));
				}

				g_imgui_rs.index_buffer = pen::renderer_create_buffer(bcp);
			}

			u32 vb_offset = 0;
			u32 ib_offset = 0;

			for (int n = 0; n < draw_data->CmdListsCount; n++)
			{
				ImDrawList* cmd_list = draw_data->CmdLists[n];
				u32 vertex_size = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
				u32 index_size = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);

				c8* vb_mem = (c8*)g_imgui_rs.vb_copy_buffer;
				c8* ib_mem = (c8*)g_imgui_rs.ib_copy_buffer;

				pen::memory_cpy(&vb_mem[vb_offset], cmd_list->VtxBuffer.Data, vertex_size);
				pen::memory_cpy(&ib_mem[ib_offset], cmd_list->IdxBuffer.Data, index_size);

				vb_offset += vertex_size;
				ib_offset += index_size;
			}

			pen::renderer_update_buffer(g_imgui_rs.vertex_buffer, g_imgui_rs.vb_copy_buffer, vb_offset);
			pen::renderer_update_buffer(g_imgui_rs.index_buffer, g_imgui_rs.ib_copy_buffer, ib_offset);

			float L = 0.0f;
			float R = ImGui::GetIO().DisplaySize.x;
			float B = ImGui::GetIO().DisplaySize.y;
			float T = 0.0f;
			float mvp[4][4] =
			{
				{ 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.5f, 0.0f },
				{ (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
			};

			pen::renderer_update_buffer(g_imgui_rs.constant_buffer, mvp, sizeof(mvp), 0);
		}

		void render(ImDrawData* draw_data)
		{
			update_dynamic_buffers(draw_data);

			pen::renderer_set_rasterizer_state(g_imgui_rs.raster_state);
			pen::renderer_set_blend_state(g_imgui_rs.blend_state);
			pen::renderer_set_depth_stencil_state(g_imgui_rs.depth_stencil_state);
            
            pmfx::set_technique(g_imgui_rs.imgui_shader, 0);

			pen::renderer_set_vertex_buffer(g_imgui_rs.vertex_buffer, 0, sizeof(ImDrawVert), 0);
			pen::renderer_set_index_buffer(g_imgui_rs.index_buffer, PEN_FORMAT_R16_UINT, 0);

			pen::renderer_set_constant_buffer(g_imgui_rs.constant_buffer, 0, PEN_SHADER_TYPE_VS);

			int vtx_offset = 0;
			int idx_offset = 0;
			for (int n = 0; n < draw_data->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[n];
				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
					if (pcmd->UserCallback)
					{
						pcmd->UserCallback(cmd_list, pcmd);
					}
					else
					{
						pen::renderer_set_texture(*(u32*)pcmd->TextureId, g_imgui_rs.font_sampler_state, 0, PEN_SHADER_TYPE_PS);

						pen::rect r = { pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w };

						pen::renderer_set_scissor_rect(r);

						pen::renderer_draw_indexed(pcmd->ElemCount, idx_offset, vtx_offset, PEN_PT_TRIANGLELIST);
					}
					idx_offset += pcmd->ElemCount;
				}
				vtx_offset += cmd_list->VtxBuffer.Size;
			}
		}

		void create_render_states()
		{
			//raster state
			rasteriser_state_creation_params rcp;
			memory_zero(&rcp, sizeof(rasteriser_state_creation_params));
			rcp.fill_mode = PEN_FILL_SOLID;
			rcp.cull_mode = PEN_CULL_NONE;
			rcp.depth_bias_clamp = 0.0f;
			rcp.sloped_scale_depth_bias = 0.0f;
			rcp.scissor_enable = true;
			rcp.depth_clip_enable = true;

			g_imgui_rs.raster_state = pen::renderer_create_rasterizer_state(rcp);

			//create a sampler object so we can sample a texture
			pen::sampler_creation_params scp;
			pen::memory_zero(&scp, sizeof(pen::sampler_creation_params));
			scp.filter = PEN_FILTER_MIN_MAG_MIP_LINEAR;
			scp.address_u = PEN_TEXTURE_ADDRESS_WRAP;
			scp.address_v = PEN_TEXTURE_ADDRESS_WRAP;
			scp.address_w = PEN_TEXTURE_ADDRESS_WRAP;
			scp.comparison_func = PEN_COMPARISON_ALWAYS;
			scp.min_lod = 0.0f;
			scp.max_lod = 0.0f;

			g_imgui_rs.font_sampler_state = pen::renderer_create_sampler(scp);

			//constant buffer
			pen::buffer_creation_params bcp;
			bcp.usage_flags = PEN_USAGE_DYNAMIC;
			bcp.bind_flags = PEN_BIND_CONSTANT_BUFFER;
			bcp.cpu_access_flags = PEN_CPU_ACCESS_WRITE;
			bcp.buffer_size = sizeof(float) * 16;
			bcp.data = (void*)nullptr;

			g_imgui_rs.constant_buffer = pen::renderer_create_buffer(bcp);

			//blend state
			pen::render_target_blend rtb;
			rtb.blend_enable = 1;
			rtb.blend_op = PEN_BLEND_OP_ADD;
			rtb.blend_op_alpha = PEN_BLEND_OP_ADD;
			rtb.dest_blend = PEN_BLEND_INV_SRC_ALPHA;
			rtb.src_blend = PEN_BLEND_SRC_ALPHA;
			rtb.dest_blend_alpha = PEN_BLEND_INV_SRC_ALPHA;
			rtb.src_blend_alpha = PEN_BLEND_SRC_ALPHA;
			rtb.render_target_write_mask = 0x0F;

			pen::blend_creation_params blend_params;
			blend_params.alpha_to_coverage_enable = 0;
			blend_params.independent_blend_enable = 0;
			blend_params.render_targets = &rtb;
			blend_params.num_render_targets = 1;

			g_imgui_rs.blend_state = pen::renderer_create_blend_state(blend_params);

			//depth stencil state
			pen::depth_stencil_creation_params depth_stencil_params = { 0 };

			// Depth test parameters
			depth_stencil_params.depth_enable = true;
			depth_stencil_params.depth_write_mask = 1;
			depth_stencil_params.depth_func = PEN_COMPARISON_ALWAYS;

			g_imgui_rs.depth_stencil_state = pen::renderer_create_depth_stencil_state(depth_stencil_params);
		}

		void process_input()
		{
			ImGuiIO& io = ImGui::GetIO();

			// mouse wheel state
			const pen::mouse_state& ms = pen::input_get_mouse_state();

			io.MouseDown[0] = ms.buttons[PEN_MOUSE_L];
			io.MouseDown[1] = ms.buttons[PEN_MOUSE_R];
			io.MouseDown[2] = ms.buttons[PEN_MOUSE_M];

			static f32 prev_mouse_wheel = (f32)ms.wheel;

			io.MouseWheel += (f32)ms.wheel - prev_mouse_wheel;
			io.MousePos.x = (f32)ms.x;
			io.MousePos.y = (f32)ms.y;

			prev_mouse_wheel = (f32)ms.wheel;

			// ascii keys
			for (u32 i = 0; i < 512; ++i)
			{
				io.KeysDown[i] = INPUT_PKEY(i);
			}

			for (u32 i = 0; i < 512; ++i)
			{
				if (pen::input_get_unicode_key(i))
				{
					io.AddInputCharacter(i);
				}
			}

			// Read keyboard modifiers inputs
			io.KeyCtrl = INPUT_PKEY(PENK_CONTROL);
			io.KeyShift = INPUT_PKEY(PENK_SHIFT);
			io.KeyAlt = INPUT_PKEY(PENK_MENU);
			io.KeySuper = false;
		}

		void new_frame()
		{
			process_input();

			ImGuiIO& io = ImGui::GetIO();

			//set delta time
			f32 cur_time = pen::timer_get_time();
			static f32 prev_time = cur_time;
			io.DeltaTime = (cur_time - prev_time) / 1000.0f;
			prev_time = cur_time;

			io.DisplaySize = ImVec2((f32)pen_window.width, (f32)pen_window.height);

			// Hide OS mouse cursor if ImGui is drawing it
			if (io.MouseDrawCursor)
				pen::input_show_cursor(false);

			ImGui::NewFrame();  
		}
        
        u32 want_capture( )
        {
            ImGuiIO& io = ImGui::GetIO();
            
            u32 flags = 0;
            
            if( io.WantCaptureMouse )
                flags |= dev_ui::MOUSE;
            
            if( io.WantCaptureMouse )
                flags |= dev_ui::MOUSE;
            
            if( io.WantCaptureMouse )
                flags |= dev_ui::MOUSE;
            
            return flags;
        }

		void render()
		{
			ImGui::Render();
		}
	}
}

