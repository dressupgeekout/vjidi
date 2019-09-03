/*
 * luamidi.c
 */

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/midiio.h>

#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>

#define SEQUENCER_DEVICE "/dev/music"

static int sequencer_fd;

static int lua_openmididevice(lua_State *L);
static int lua_closemididevice(lua_State *L);
static int lua_timingstart(lua_State *L);
static int lua_timingstop(lua_State *L);
static int lua_tempo(lua_State *L);
static int lua_timesig(lua_State *L);
static int lua_noteon(lua_State *L);
static int lua_noteoff(lua_State *L);
static int lua_waitrel(lua_State *L);

static luaL_Reg functions[] = {
	{"openmididevice", lua_openmididevice},
	{"closemididevice", lua_closemididevice},
	{"tempo", lua_tempo},
	{"timingstart", lua_timingstart},
	{"timingstop", lua_timingstop},
	{"timesig", lua_timesig},
	{"noteon", lua_noteon},
	{"noteoff", lua_noteoff},
	{"waitrel", lua_waitrel},
	{NULL, NULL},
};

static int
lua_openmididevice(lua_State *L)
{
	sequencer_fd = open(SEQUENCER_DEVICE, O_RDWR);
	return 0;
}

static int
lua_closemididevice(lua_State *L)
{
	close(sequencer_fd);
	return 0;
}

static int
lua_timingstart(lua_State *L)
{
	write(sequencer_fd, &SEQ_MK_TIMING(START), sizeof(seq_event_t));
}


static int
lua_timingstop(lua_State *L)
{
	write(sequencer_fd, &SEQ_MK_TIMING(STOP), sizeof(seq_event_t));
}


/*
 * E.g. midi.tempo(120);
 */
static int
lua_tempo(lua_State *L)
{
	int tempo = lua_tointeger(L, 1);
	write(sequencer_fd, &SEQ_MK_TIMING(TEMPO, .bpm=tempo), sizeof(seq_event_t));
	return 0;
}

/*
 * E.g. midi.timesig(4, 4)
 */
static int
lua_timesig(lua_State *L)
{
	int num = lua_tointeger(L, 1);
	int den = lua_tointeger(L, 2);
	//write(sequencer_fd, &SEQ_MK_TIMING(TIMESIG, .numerator=num, .lg2denom=log2((double)den), .clks_per_click=24, .dsq_per_24clks=8), sizeof(seq_event_t));
	write(sequencer_fd, &SEQ_MK_TIMING(TIMESIG, .numerator=num, .lg2denom=den, .clks_per_click=24, .dsq_per_24clks=8), sizeof(seq_event_t));
	return 0;
}

/*
 * E.g. midi.noteon(key, velocity)
 */
static int
lua_noteon(lua_State *L)
{
	int key = lua_tointeger(L, 1);
	int velocity = lua_tointeger(L, 2);
	write(sequencer_fd, &SEQ_MK_CHN(NOTEON, .device=1, .channel=0, .key=key, .velocity=velocity), sizeof(seq_event_t));
	return 0;
}

static int
lua_noteoff(lua_State *L)
{
	int key = lua_tointeger(L, 1);
	write(sequencer_fd, &SEQ_MK_CHN(NOTEOFF, .device=1, .channel=0, .key=key, .velocity=1), sizeof(seq_event_t));
	return 0;
}

static int
lua_waitrel(lua_State *L)
{
	int amount;

	if (lua_gettop(L) >= 1) {
		amount = lua_tointeger(L, 1);
	} else {
		amount = 24;
	}
	
	write(sequencer_fd, &SEQ_MK_TIMING(WAIT_REL, .divisions=amount), sizeof(seq_event_t));
	return 0;
}

int
luaopen_luamidi(lua_State *L)
{
#if 0
	// If Lua >= 5.3:
	luaL_newlib(L, functions);
#endif

	luaL_register(L, "midi", functions);

	return 1;
}

