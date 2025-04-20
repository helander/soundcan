#include "ports.h"
#include "constants.h"
#include "midifile.h"
#include <spa/control/control.h>
#include <spa/pod/builder.h>

static float dummyAudioInput[20000];
static float dummyAudioOutput[20000];

#define ATOM_BUFFER_SIZE 8192

// Dummy routines (noop - no operations)
static void clear_noop(LilvInstance* instance, struct lph_port* this) {}
static void pre_run_noop(LilvInstance* instance, struct lph_port* this, uint64_t frame, float denom, uint64_t n_samples) {}
static void post_run_noop(LilvInstance* instance, struct lph_port* this) {}


//============================= control input ==========================================================

static void setup_control_input(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    lilv_instance_connect_port(instance, this->index, &this->port_data.control_input.current);
}

static void init_control_input(struct lph_port* this)
{
    this->type = CONTROL_INPUT;
    this->methods.setup = setup_control_input;
}

//============================= control output ==========================================================

static void setup_control_output(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    lilv_instance_connect_port(instance, this->index, &this->port_data.control_output.current);
}

static void init_control_output(struct lph_port* this)
{
    this->type = CONTROL_OUTPUT;
    this->methods.setup = setup_control_output;
}

//============================ audio input ===========================================================

static void setup_audio_input(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    this->pwPort = pw_filter_add_port(
        filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS, 0,
        pw_properties_new(PW_KEY_FORMAT_DSP, "32 bit float mono audio", PW_KEY_PORT_NAME, this->name, NULL),
        NULL, 0);
}

static void pre_run_audio_input(LilvInstance* instance, struct lph_port* this, uint64_t frame, float denom, uint64_t n_samples)
{
   float *inp = pw_filter_get_dsp_buffer(this->pwPort, n_samples);
   if (inp == NULL) {
        lilv_instance_connect_port(instance, this->index, dummyAudioOutput);
   } else {
        lilv_instance_connect_port(instance, this->index, inp);
   }
}

static void init_audio_input(struct lph_port* this)
{
    this->type = AUDIO_INPUT;
    this->methods.setup = setup_audio_input;
    this->methods.pre_run = pre_run_audio_input;
}

//============================= audio output ==========================================================

static void setup_audio_output(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    this->pwPort = pw_filter_add_port(
        filter, PW_DIRECTION_OUTPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS, 0,
        pw_properties_new(PW_KEY_FORMAT_DSP, "32 bit float mono audio", PW_KEY_PORT_NAME, this->name, NULL),
        NULL, 0);
}

static void pre_run_audio_output(LilvInstance* instance, struct lph_port* this, uint64_t frame, float denom, uint64_t n_samples)
{
   float *outp = pw_filter_get_dsp_buffer(this->pwPort, n_samples);
   if (outp == NULL) {
        lilv_instance_connect_port(instance, this->index, dummyAudioOutput);
   } else {
        lilv_instance_connect_port(instance, this->index, outp);
   }
}

static void init_audio_output(struct lph_port* this)
{
    this->type = AUDIO_OUTPUT;
    this->methods.setup = setup_audio_output;
    this->methods.pre_run = pre_run_audio_output;
}

//====================== atom input =================================================================

static void setup_atom_input(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    this->port_data.atom_input.buffer = calloc(1, ATOM_BUFFER_SIZE);
    lilv_instance_connect_port(instance, this->index, this->port_data.atom_input.buffer);

    this->pwPort = pw_filter_add_port(
        filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS, 0,
        pw_properties_new(PW_KEY_FORMAT_DSP, "8 bit raw midi", PW_KEY_PORT_NAME, this->name, NULL), NULL,
        0);
}

static void pre_run_atom_input(LilvInstance* instance, struct lph_port* this, uint64_t frame, float denom, uint64_t n_samples)
{
    this->pwbuffer = pw_filter_dequeue_buffer(this->pwPort);

    if (!this->pwbuffer)
        return;

    LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence*)this->port_data.atom_input.buffer;
    aseq->atom.size = ATOM_BUFFER_SIZE - sizeof(LV2_Atom);
    lv2_atom_sequence_clear(aseq);
    struct spa_buffer* buf;
    struct spa_data* d;
    struct spa_pod* pod;
    struct spa_pod_control* c;
    buf = this->pwbuffer->buffer;
    d = &buf->datas[0];
    if ((pod = spa_pod_from_data(d->data, d->maxsize, d->chunk->offset, d->chunk->size)) != NULL) {
        if (spa_pod_is_sequence(pod)) {
            int buf_offset = 0;
            SPA_POD_SEQUENCE_FOREACH((struct spa_pod_sequence*)pod, c)
            {
                struct midi_event ev;
                if (c->type != SPA_CONTROL_Midi)
                    continue;
                ev.track = 0;
                ev.sec = (frame + c->offset) / denom;
                ev.data = SPA_POD_BODY(&c->value);
                ev.size = SPA_POD_BODY_SIZE(&c->value);
		if (ev.data[0] == 0xf8) continue;
		  if (ATOM_BUFFER_SIZE - sizeof(LV2_Atom) - aseq->atom.size >= sizeof(LV2_Atom_Event) + ev.size) {
  			LV2_Atom_Event* aev = (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq) + buf_offset);
  			aev->time.frames = ev.sec;
  			aev->body.type   = constants.midi_MidiEvent;
  			aev->body.size   = ev.size;
  			memcpy(LV2_ATOM_BODY(&aev->body), ev.data, ev.size);

  			int size = lv2_atom_pad_size(sizeof(LV2_Atom_Event) + ev.size);
  			aseq->atom.size += size;
  			buf_offset += size;
                }
            }
        } else {
            pw_log_error("Pod is not sequence port %d", this->index);
        }
    } else {
        pw_log_error("No pod for port %d", this->index);
    }
}

static void post_run_atom_input(LilvInstance* instance, struct lph_port* this)
{
    if (this->pwbuffer)
        pw_filter_queue_buffer(this->pwPort, this->pwbuffer);
}

static void init_atom_input(struct lph_port* this)
{
    this->type = ATOM_INPUT;
    this->methods.setup = setup_atom_input;
    this->methods.pre_run = pre_run_atom_input;
    this->methods.post_run = post_run_atom_input;
}

//============================ atom output ===========================================================

static void setup_atom_output(LilvInstance* instance, struct lph_port* this, struct pw_filter* filter)
{
    this->port_data.atom_output.buffer = calloc(1, ATOM_BUFFER_SIZE);
    lilv_instance_connect_port(instance, this->index, this->port_data.atom_output.buffer);
    this->pwPort = pw_filter_add_port(
        filter, PW_DIRECTION_OUTPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS, 0,
        pw_properties_new(PW_KEY_FORMAT_DSP, "8 bit raw midi", PW_KEY_PORT_NAME, this->name, NULL), NULL,
        0);
}

static void pre_run_atom_output(LilvInstance* instance, struct lph_port* this, uint64_t frame, float denom, uint64_t n_samples)
{
    LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence*)this->port_data.atom_output.buffer;
    aseq->atom.size = ATOM_BUFFER_SIZE - sizeof(LV2_Atom);
    aseq->atom.type = constants.atom_Chunk;

    this->pwbuffer = pw_filter_dequeue_buffer(this->pwPort);
}

static void post_run_atom_output(LilvInstance* instance, struct lph_port* this)
{
    if (!this->pwbuffer) return;
        LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence*)this->port_data.atom_output.buffer;
        LV2_Atom_Event* aev = (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));
        if (aseq->atom.size > sizeof(LV2_Atom_Sequence)) {
            struct spa_data* d;
            struct spa_pod_builder builder;
            struct spa_pod_frame frame;

            spa_assert(this->pwbuffer->buffer->n_datas == 1);
            d = &this->pwbuffer->buffer->datas[0];
            d->chunk->offset = 0;
            d->chunk->size = 0;
            d->chunk->stride = 1;
            d->chunk->flags = 0;
            spa_pod_builder_init(&builder, d->data, d->maxsize);
            spa_pod_builder_push_sequence(&builder, &frame, 0);
            long payloadSize = aseq->atom.size;
            while (payloadSize > (long)sizeof(LV2_Atom_Event)) {
                if (aev->body.type == constants.midi_MidiEvent) {
                    uint8_t* mididata = (uint8_t*)aev + sizeof(LV2_Atom_Event);
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_Midi);
                    spa_pod_builder_bytes(&builder, mididata, aev->body.size);
                }
                int eventSize = lv2_atom_pad_size(sizeof(LV2_Atom_Event)) + lv2_atom_pad_size(aev->body.size);
                char* next = ((char*)aev) + eventSize;
                payloadSize = payloadSize - eventSize;
                aev = (LV2_Atom_Event*)next;
            }
            spa_pod_builder_pop(&builder, &frame);
            d->chunk->size = builder.state.offset;
        }

        pw_filter_queue_buffer(this->pwPort, this->pwbuffer);
}

static void init_atom_output(struct lph_port* this)
{
    this->type = ATOM_OUTPUT;
    this->methods.setup = setup_atom_output;
    this->methods.pre_run = pre_run_atom_output;
    this->methods.post_run = post_run_atom_output;
}

void init_ports(const LilvPlugin* plugin, struct lph_ports* ports)
{
    memset(dummyAudioInput, 0, sizeof(dummyAudioInput));

    int n_ports = lilv_plugin_get_num_ports(plugin);
    ports->n_ports = n_ports;
    for (int n = 0; n < n_ports; n++) {
        struct lph_port* port = &ports->ports[n];
        port->index = n;
        port->lilvPort = lilv_plugin_get_port_by_index(plugin, n);
        strcpy(port->name, lilv_node_as_string(lilv_port_get_symbol(plugin, port->lilvPort)));
        port->pwPort = NULL;
	port->methods.clear = clear_noop;
	port->methods.pre_run = pre_run_noop;
	port->methods.post_run = post_run_noop;
        if (lilv_port_is_a(plugin, port->lilvPort, constants.atom_AtomPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_InputPort)) {
            init_atom_input(port);
        } else if (lilv_port_is_a(plugin, port->lilvPort, constants.atom_AtomPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_OutputPort)) {
            init_atom_output(port);
        } else if (lilv_port_is_a(plugin, port->lilvPort, constants.lv2_ControlPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_InputPort)) {
            init_control_input(port);
        } else if (lilv_port_is_a(plugin, port->lilvPort, constants.lv2_ControlPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_OutputPort)) {
            init_control_output(port);
        } else if (lilv_port_is_a(plugin, port->lilvPort, constants.lv2_AudioPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_InputPort)) {
            init_audio_input(port);
        } else if (lilv_port_is_a(plugin, port->lilvPort, constants.lv2_AudioPort) && lilv_port_is_a(plugin, port->lilvPort, constants.lv2_OutputPort)) {
            init_audio_output(port);
        } else {
            printf("\nUnsupported port type: port #%d (%s)", port->index, port->name);
        }
    }
}
