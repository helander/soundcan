
#define ATOM_BUFFER_SIZE 0x2000

struct wip_plugin {
	const struct wip_descriptor *(*make_desc)(struct wip_plugin *plugin, const char *name);
	void (*unload) (struct wip_plugin *plugin);
};

struct wip_port {
	uint32_t index;
	const char *name;
	void *pwport;
#define WIP_PORT_INPUT	(1ULL << 0)
#define WIP_PORT_OUTPUT	(1ULL << 1)
#define WIP_PORT_CONTROL	(1ULL << 2)
#define WIP_PORT_AUDIO	(1ULL << 3)
	uint64_t flags;

//#define FC_HINT_BOOLEAN		(1ULL << 2)
//#define FC_HINT_SAMPLE_RATE	(1ULL << 3)
//#define FC_HINT_INTEGER		(1ULL << 5)
	uint64_t hint;
	float def;
	float min;
	float max;
	uint8_t atomBuffer[ATOM_BUFFER_SIZE];
};

#define WIP_IS_PORT_INPUT(x)	((x) & WIP_PORT_INPUT)
#define WIP_IS_PORT_OUTPUT(x)	((x) & WIP_PORT_OUTPUT)
#define WIP_IS_PORT_CONTROL(x)	((x) & WIP_PORT_CONTROL)
#define WIP_IS_PORT_AUDIO(x)	((x) & WIP_PORT_AUDIO)

struct wip_descriptor {
	const char *name;
//#define FC_DESCRIPTOR_SUPPORTS_NULL_DATA	(1ULL << 0)
//#define FC_DESCRIPTOR_COPY			(1ULL << 1)
	uint64_t flags;

	void (*free) (const struct wip_descriptor *desc);

	uint32_t n_ports;
	struct wip_port *ports;

	void *(*instantiate) (const struct wip_descriptor *desc,
			unsigned long SampleRate, int index, const char *config);

	void (*cleanup) (void *instance);

	void (*connect_port) (void *instance, unsigned long port, float *data);
	void (*control_changed) (void *instance);

	void (*activate) (void *instance);
	void (*deactivate) (void *instance);

	void (*run) (void *instance, unsigned long SampleCount);
};


static inline void wip_plugin_free(struct wip_plugin *plugin)
{
	if (plugin->unload)
		plugin->unload(plugin);
}

static inline void wip_descriptor_free(const struct wip_descriptor *desc)
{
	if (desc->free)
		desc->free(desc);
}

extern struct wip_plugin *wip_plugin_load(const char *plugin_uri);

extern void midiport_write(struct wip_port *port, uint8_t *mididata, int midibytes, long evtime, int *offset);
extern void midiport_clear(struct wip_port *port);

