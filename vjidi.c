/*
 * vjidi.c
 * Charlotte Koch <cfkoch@edgebsd.org>
 */

#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <SDL2/SDL.h>

#include <sys/types.h>
#include <sys/midiio.h>

#define DEFAULT_SEQUENCER_DEVICE "/dev/music"

static bool debug = false;
static int sequencer_fd;
static seq_event_t *midi_event = NULL;
static lua_State *L = NULL;
static char *script = NULL;

/* */

static int __noop(lua_State *L);
static void pushmidievent(lua_State *L, const seq_event_t *ev);

/* */

static int parse_options(int argc, char *argv[]);
static void usage(void);
static bool setup_events(void);
static bool setup_script_interpreter(void);
static void handle_sigint(int sig);
static void cleanup(void);

/* */

int
main(int argc, char *argv[])
{
	signal(SIGINT, handle_sigint);

	int rv;

	if ((rv = parse_options(argc, argv)) >= 0)
		return rv;

	if (!setup_events())
		goto fail;

	if (!setup_script_interpreter())
		goto fail;

	sequencer_fd = open(DEFAULT_SEQUENCER_DEVICE, O_RDONLY);

	if ((midi_event = malloc(sizeof(seq_event_t))) == NULL)
		goto fail;

	for (;;) {
		read(sequencer_fd, midi_event, sizeof(seq_event_t));

		switch (midi_event->tag) {
		case SEQ_CHN_VOICE:
			switch (midi_event->voice.op) {
			case MIDI_NOTEON:
				lua_getglobal(L, "onnotedown");
				pushmidievent(L, midi_event);
				lua_pcall(L, 1, 0, 0);
				if (debug)
					warnx("ON\tkey=%d velocity=%d", midi_event->voice.key, midi_event->c_NOTEON.velocity);
				break;
			case MIDI_NOTEOFF:
				lua_getglobal(L, "onnoteup");
				pushmidievent(L, midi_event);
				lua_pcall(L, 1, 0, 0);
				if (debug)
					warnx("OFF\tkey=%d", midi_event->voice.key);
				break;
			}
			break;
		case SEQ_CHN_COMMON:
			switch (midi_event->common.op) {
			case MIDI_CTL_CHANGE:
				lua_getglobal(L, "oncontrol");
				pushmidievent(L, midi_event);
				lua_pcall(L, 1, 0, 0);
				if (debug)
					warnx("CTL_CHANGE\tdevice=%u channel=%u controller=%u value=%u",
						midi_event->c_CTL_CHANGE.device,
						midi_event->c_CTL_CHANGE.channel,
						midi_event->c_CTL_CHANGE.controller,
						midi_event->c_CTL_CHANGE.value);
				break;
			}
			break;
		default:
			warnx("(other)");
		}
	}

success:
	cleanup();
	return EXIT_SUCCESS;

fail:
	cleanup();
	return EXIT_FAILURE;
}

static int
parse_options(int argc, char *argv[])
{
	int ch;

	while ((ch = getopt(argc, argv, "Dh")) != -1) {
		switch (ch) {
		case 'D':
			debug = true;
			break;
		case 'h':
			usage();
			return EXIT_SUCCESS;
			break;
		case '?': /* FALLTHROUGH */
		default:
			usage();
			return EXIT_FAILURE;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1) {
		usage();
		return EXIT_FAILURE;
	}

	script = argv[0];

	return -1;
}

static void
usage(void)
{
	warnx("usage: %s [-Dh] script", getprogname());
}

static bool
setup_events(void)
{
	SDL_Init(SDL_INIT_EVERYTHING); // XXX not really
	return true;
}

static bool
setup_script_interpreter(void)
{
	if ((L = luaL_newstate()) == NULL)
		return false;

	luaL_openlibs(L);
	lua_register(L, "onnoteup", __noop);
	lua_register(L, "onnotedown", __noop);
	lua_register(L, "oncontrol", __noop);

	if (luaL_dofile(L, script)) {
		warnx("%s", lua_tostring(L, 1));
		return false;
	}

	return true;
}

static void
handle_sigint(int sig)
{
	cleanup();
	exit(EXIT_SUCCESS);
}

static void
cleanup(void)
{
	SDL_Quit();
	lua_close(L);
	close(sequencer_fd);
	free(midi_event);
}

/* */

/*
 * Deliberate noop so that callbacks not explicitly setup by the user don't
 * freak out the Lua interpreter.
 */
static int
__noop(lua_State *L)
{
	return 0;
}

static void
pushmidievent(lua_State *L, const seq_event_t *ev)
{
	lua_newtable(L);

	switch (midi_event->tag) {
	case SEQ_CHN_VOICE:
		break;
	case SEQ_CHN_COMMON:
		{
			struct { const char *k; unsigned int v; } numbers[] = {
				{ "controller", ev->c_CTL_CHANGE.controller },
				{ "device", ev->c_CTL_CHANGE.device },
				{ "value", ev->c_CTL_CHANGE.value },
				{ NULL, 0 },
			};

			int i;

			for (i = 0; numbers[i].k != NULL; i++) {
				lua_pushstring(L, numbers[i].k);
				lua_pushinteger(L, numbers[i].v);
				lua_settable(L, -3);
			}
		}
		break;
	}
}
