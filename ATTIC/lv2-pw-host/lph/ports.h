#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/forge.h>
#include <lv2/atom/util.h>
#include <lv2/buf-size/buf-size.h>
#include <lv2/midi/midi.h>
#include <lv2/options/options.h>
#include <lv2/parameters/parameters.h>
#include <lv2/worker/worker.h>

#include <pipewire/filter.h>
#include <pipewire/pipewire.h>


typedef enum
{
    CONTROL_INPUT,
    CONTROL_OUTPUT,
    AUDIO_INPUT,
    AUDIO_OUTPUT,
    ATOM_INPUT,
    ATOM_OUTPUT
} lph_port_type;

struct lph_port;

struct control_input_port
{
    float current;
};

struct control_output_port
{
    float current;
};

struct audio_input_port
{
};

struct audio_output_port
{
};

struct atom_input_port
{
    LV2_Atom_Sequence *buffer;
};

struct atom_output_port
{
    LV2_Atom_Sequence *buffer;
};

struct lph_port_methods
{
    void (*setup)(LilvInstance *instance, struct lph_port *this, struct pw_filter *filter);
    void (*clear)(LilvInstance *instance, struct lph_port *this);
    void (*pre_run)(LilvInstance *instance, struct lph_port *this, uint64_t frame, float denom, uint64_t n_samples);
    void (*post_run)(LilvInstance *instance, struct lph_port *this);
};

struct lph_port
{
    int index;
    lph_port_type type;
    char name[100];
    float dfault;
    float min;
    float max;
    struct lph_port_methods methods;
    const LilvPort *lilvPort;
    void *pwPort;
    struct pw_buffer  *pwbuffer;
    union {
        struct control_input_port control_input;
        struct control_output_port control_output;
        struct audio_input_port audio_input;
        struct audio_output_port audio_output;
        struct atom_input_port atom_input;
        struct atom_output_port atom_output;
    } port_data;
};

struct lph_ports
{
    int n_ports;
    struct lph_port ports[100];
};

extern void init_ports(const LilvPlugin *plugin, struct lph_ports *ports);
