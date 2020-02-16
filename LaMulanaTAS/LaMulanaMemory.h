#pragma once
#include "util.h"
#include "d3d9.h"

class LaMulanaMemory
{
	char *base;
public:
	LaMulanaMemory(char *base_) : base(base_) {}

	struct drawdata {
		float w, h;
		unsigned vert_rgba[4];
		int unk18, unk1c;
		float u1, v1, u2, v2;
		IDirect3DTexture9 *tex;
		char unk34[0x18];
		int unk4c;
		float texel_w, texel_h;
	};

	union object
	{
		// IMPORTANT: This is just a common structure for objects, most fields are unimplemented for any given object
		// There is a lot of guesswork here, don't assume I know what I'm talking about
		char raw[0x334];
		struct {
			void (*create)(object*);
			bool (*update)(object*);
			void (*drawlist)(object*);
			bool (*update_postcoldet)(object*);
			void (*draw)(object*);
			void (*destroy)(object*);
			void (*hitbox)(object*);
			int state;
			// although I'm calling them locals, there is other private storage in objects
			// arguments passed at construction time go into these three arrays
			int local_int[32];
			void *local_ptr[16];
			int private_int[32]; // no arguments, pure local storage
			float local_float[32];
			float x, y, z; // usually screen coordinates
			// I think only lemeza and moving platforms even implement these, also why is there a prev_z?
			float prev_x, prev_y, prev_z;
			// don't pay attention to these names, beyond being xy I don't really know
			// sometimes it seems to be room coordinates when x,y are screen coordinates but sometimes not
			// also z is just my inference, nothing uses it
			float room_x, room_y, room_z;
			int hp;
			drawdata drawdata;
			unsigned anim_frame;
			unsigned transformation;
			int unk268; // never used, no clue
			unsigned char test_count;
			struct byteop *tests;
			int processing_flags;
			char unk278[0x2d8-0x278]; // a bunch of this is somewhat known actually
			short nextidx, previdx; // used for finding free slots for allocation
			short priority;
			int idx;
			char unk2e4[8]; // actually not unknown at all, sorry
			object *next, *prev; // used for processing order within a priority
			char db[3]; // I have specifics but they're wrong
			char zone, room, screen;
			char unk2fa[0x308-0x2fa];
			int draw_priority;
			int unk30c;
			int screen_x, screen_y; // definitely ignore these names apart from the x/y part
			int unk318[6]; // performance counters I think
			// set to the same as create during allocation, but never used
			// you'd think this means it's for rtti but create is used for that too
			void (*create2)(object*);
		};
	};
	static_assert(offsetof(object, create2) == 0x330, "Error in object structure");

	struct hitbox
	{
		float x, y, w, h;
		int damage, type;
		object *object;
		int unk2, unk3, unk4;
	};

	struct mapid
	{
		char zone, room, screen;
	};

	struct screen
	{
		wchar_t *name;
		unsigned char unk04;
		union
		{
			struct
			{
				mapid up, right, down, left;
			};
			mapid exits[4]; // up, right, down, left
		};
		unsigned char unk11[3];
		int unk14;
	};

	struct room
	{
		unsigned char unk00;
		unsigned char layers;
		unsigned char primary_layer; // lower is foreground, higher is background
		unsigned char unk03;
		unsigned char num_screens;
		unsigned char unk05[3];
		void *gfx_tiles;
		void *unk0C;
		screen *screens;
		short w, h;
		unsigned char *hit_tiles; // w*h rowwise
	};

	// not 1:1 with fields
	struct zone
	{
		unsigned char texsheet_idx;
		unsigned char unk01;
		unsigned char num_rooms;
		unsigned char num_anims;
		unsigned char unk04[4];
		room *rooms;
		void *anims;
		void *unk10;
		int index;
	};

	struct map
	{
		int num_zones;
		zone *zones;
	};

	struct scrolling {
		short unk00;
		short w_screens, h_screens;
		short unk06;
		float w_20, h_20;
		int unk10, unk14;
		float x, y;
	};

	struct solid {
		float x;
		float y;
		float w;
		float h;
		float rotdeg;
		float pivot_x;
		float pivot_y;
		float vel_x;
		float vel_y;
		int unk24;
		object *obj;
	};

	struct texture {
		IDirect3DTexture9 *tex;
		short w, h;
		int unk08, unk0C, unk10, unk14;
	};

	char &kb_enabled = *(base + 0x6d3618);
	HWND &window = *(HWND*)(base + 0xdb4b68);
	void (*&post_process)() = *(void (**)())(base + 0x6d6b64);
	D3DFORMAT &display_format = *(D3DFORMAT*)(base + 0xdb4984);
	IDirect3DSurface9 *&postprocessed_gamesurf = *(IDirect3DSurface9**)(base + 0x6d6b98);
	float &game_horz_offset = *(float*)(base + 0xdb4994);
	float &game_vert_offset = *(float*)(base + 0xdb4988);
	float &game_horz_scale = *(float*)(base + 0x6d6b90);
	float &game_vert_scale = *(float*)(base + 0x85d614);
	// Always 640x480 but the game uses these so I will too
	float &game_width = *(float*)(base + 0xdb4b24);
	float &game_height = *(float*)(base + 0x6d6af8);

	short &rng = *(short *)(base + 0x6d4a50);
	int &game_state = *(int*)(base + 0xdb4b80);
	char &has_quicksave = *(base + 0x6d2967);
	short &timeattack_cursor = *(short*)(base + 0x6d0218);
	int &lemeza_spawned = *(int*)(base + 0xdb753c);
	object *&lemeza_obj = *(object**)(base + 0xdb7538);
	unsigned (&flags1)[5] = *(unsigned (*)[5])(base + 0x6d59c0);

	char &in_timeattack = *(char*)(base + 0x6d4dd3);
	map &map_main = *(map*)(base + 0xdb4c70);
	map &map_timeattack = *(map*)(base + 0xdb5984);
	zone *&map_boss = *(zone**)(base + 0xdb5994);
	short &boss_mapidx = *(short*)(base + 0x6d59a0);
	short &boss_room = *(short*)(base + 0x6d4dd0);
	unsigned char(&tile_overlay)[64][48] = *(unsigned char(*)[64][48])(base + 0xdb4d68); //colwise
	char &cur_zone = *(char*)(base + 0xdb4bb7);
	char &cur_room = *(char*)(base + 0xdb4bb3);
	char &cur_screen = *(char*)(base + 0xdb4bb6);

	unsigned char &scroll_dbidx = *(unsigned char*)(base + 0xdb4c7a);
	scrolling (&scroll_db)[2] = *(scrolling(*)[2])(base + 0xdb5998);

	unsigned char &solids_dbidx = *(unsigned char*)(base + 0xdb4bdf);
	solid* (&solids_db)[2] = *(solid*(*)[2])(base + 0xdb4ca4);
	int (&solids_count)[2] = *(int(*)[2])(base + 0xdb4c7c);

	object* (&objects) = *(object**)(base + 0xdb7188);
	void (*(&objtypes)[204])(object*) = *(void (*(*)[204])(object*))(base + 0x6d10b8);

	void (*const kill_objects)() = (void (*)())(base + 0x6072d0);
	void (*const reset_game)() = (void (*)())(base + 0x4d9690);
	void (*const create_obj_inst_)(void (*create)(object*)) = (void (*)(void (*)(object*)))(base + 0x607570);
	void (**const sleep)(int) = (void (**)(int))(base + 0x6d4f68);

	void (*const iframes_create)(object*) = (void (*)(object*))(base + 0x5feeb0);
	void (*const startup_create)(object*) = (void (*)(object*))(base + 0x6222f0);
	void (*const pot_create)(object*) = (void (*)(object*))(base + 0x632470);
	void (*const drop_create)(object*) = (void (*)(object*))(base + 0x455960);
	void (*const mother5_create)(object*) = (void (*)(object*))(base + 0x54ea60);

	// This function is empty, I'm literally calling it just to prevent the compiler from assuming it knows what registers are and aren't destroyed wtf
	void (*const noop)() = (void (*)())(base + 0x4f6540);

	/*
	jump, main, sub, item
	ok,cancel,?,?
	f1,esc,+m,-m
	+s,-s,?,?
	-menu,+menu,up,right
	down,left,esc,f2
	f3,f4,f5,f6
	f7,f8,f9

	Unsure why there are two bound to esc by default, they must have different functions but idk what
	*/
	unsigned char * KeyMappings()
	{
		unsigned char *settings = *(unsigned char**)(base + 0xdb4b54);
		if (!settings)
			return nullptr;
		return settings + 0x5c;
	}

	IDirect3D9 * id3d9()
	{
		return *(IDirect3D9**)(base + 0xdb7548);
	}

	IDirect3DDevice9 * id3d9dev()
	{
		return *(IDirect3DDevice9**)(base + 0xdb754c);
	}

	void scrub_objects()
	{
		// the game's very bad at actually resetting things, which is related to some of its bugs
		char *objptr = (char*)objects;
		for (int i = 0; i < 0x600; i++, objptr += 820)
		{
			// see init_objects at 0x6070d0
			memset(objptr, 0, 0x280);
			memset(objptr + 0x288, 0, 0x2d8 - 0x288);
			*(int*)(objptr + 0x2e4) = 0;
			memset(objptr + 0x2e8, 0, 820 - 0x2e8);
		}
	}

	void setvsync(bool vsync)
	{
		UINT presentint = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		if (((D3DPRESENT_PARAMETERS*)(base + 0xdb49c4))->PresentationInterval != presentint)
		{
			((D3DPRESENT_PARAMETERS*)(base + 0xdb49c4))->PresentationInterval = presentint;
			*(base + 0x6d59a3) = 0; // force device reset
		}
	}

	void saveslot(int slot)
	{
		// create quick save data
		((void (*)())(base + 0x483db0))();
		// save quick save data
		((void (*)(const char *savedata, int size, const char *filename))(base + 0x4752a0))(*(char**)(base + 0x6d5a38), 16384, strprintf("lm_%.2x.sav", slot).data());
	}

	void loadslot(int slot)
	{
		static object fakeloadmenu;
		fakeloadmenu.private_int[10] = 1;
		fakeloadmenu.private_int[5] = slot / 5;
		fakeloadmenu.private_int[0] = slot % 5;
		kill_objects(); // clears objects
		((void (*)(object*))(base + 0x44aa30))(&fakeloadmenu); // loads save, intended to be called from menu code but it only references these three fields 
	}

	vararray<hitbox> gethitboxes(int type)
	{
		static const struct { off_t ptr_offset, count_offset; } hitbox_ptrs[] =
		{
			{ 0x6d5654, 0x6d4f60 },
			{ 0x6d5040, 0x6d4f84 },
			{ 0x6d56e0, 0x6d5140 },
			{ 0x6d5a60, 0x6d4d68 },
			{ 0x6d56f0, 0x6d4dbc },
			{ 0x6d4f5c, 0x6d5194 },
			{ 0x6d568c, 0x6d5198 },
			{ 0x6d5a68, 0x6d5064 },
			{ 0x6d50dc, 0x6d5a00 },
			{ 0x6d5190, 0x6d5068 },
			{ 0x6d5814, 0x6d5a64 },
			{ 0x6d5144, 0x6d56e4 }, // this one's definitely always empty but including it in the list anyway for now
			{ 0x6d4f80, 0x6d5804 },
		};
		return vararray<hitbox>(*(hitbox**)(base + hitbox_ptrs[type].ptr_offset), *(int*)(base + hitbox_ptrs[type].count_offset));
	}

	map & getmap()
	{
		return in_timeattack ? map_timeattack : map_main;
	}

	room *getroom()
	{
		if (flags1[1] & 0x400)
			return &map_boss[boss_mapidx].rooms[boss_room];
		else if (cur_zone >= 0)
			return &getmap().zones[cur_zone].rooms[cur_room];
		return nullptr;
	}

	unsigned char gettile_map(int x, int y)
	{
		room *here = getroom();
		return here ? here->hit_tiles[y * here->w + x] : 0;
	}

	unsigned char gettile_effective(int x, int y)
	{
		const auto &scroll = scroll_db[scroll_dbidx];
		int ov_x = x - (int)(scroll.x * 0.1f);
		int ov_y = y - (int)(scroll.y * 0.1f);
		unsigned char overlay = (ov_x >= 0 && ov_x < 64 && ov_y >= 0 && ov_y < 48) ? tile_overlay[ov_x][ov_y] : 0;
		unsigned char maptile = gettile_map(x, y);
		if (flags1[2] & 0x4000 || overlay >= 0x7f || overlay != 0 && maptile < 0x80)
			return overlay;
		return gettile_map(x, y);
	}

	vararray<solid> getsolids()
	{
		return vararray<solid>(solids_db[solids_dbidx ^ 1], solids_count[solids_dbidx ^ 1]);
	}

	void loadgfx(const char *filename, texture *tex)
	{
		auto loadgfx_ = (void (*)())(base + 0x458e30);
		__asm {
			mov eax, loadgfx_;
			mov ecx, filename;
			mov esi, tex;
			call eax;
		}
		noop();
	}

	class objfixup {
		LaMulanaMemory *memory;
		int type;
		std::vector<std::pair<int, unsigned char>> data;
		static std::vector<std::pair<int, unsigned char>> data_f;
		void(*orig_create)(object*);
		static void(*orig_create_f)(object*);
	public:
		objfixup(LaMulanaMemory *memory, int type, std::vector<std::pair<int, unsigned char>> data) : memory(memory), type(type), data(data) {}
		static void fixup_create(object* self)
		{
			for (auto i : data_f)
				self->raw[i.first] = i.second;
			orig_create_f(self);
		}
		void inject() {
			orig_create_f = orig_create = memory->objtypes[type];
			memory->objtypes[type] = fixup_create;
			data_f = data;
		}
		void remove() {
			memory->objtypes[type] = orig_create;
		}
	};
};
