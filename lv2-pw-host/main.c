
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <signal.h>

#include <spa/pod/builder.h>
#include <spa/param/latency-utils.h>

#include <pipewire/pipewire.h>
#include <pipewire/filter.h>


#include <spa/utils/result.h>
#include <spa/utils/defs.h>
#include <spa/control/control.h>
#include <spa/param/audio/format-utils.h>
#include <spa/param/props.h>

#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>


#include "midifile.h"
#include "wip_lv2host.h"

struct data;

struct port {
	struct data *data;
	unsigned long pluginPort;
};

struct data {
	struct pw_main_loop *loop;
	struct pw_filter *filter;
	int64_t clock_time;
	const struct wip_plugin *plugin;
	const struct wip_descriptor *desc;
	void *instance;
};

static void on_process(void *userdata, struct spa_io_position *position)
{
	struct data *data = userdata;
	uint32_t n_samples = position->clock.duration;
	uint64_t frame = data->clock_time;

	data->clock_time += position->clock.duration;

	//Process inputs
	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		if (WIP_IS_PORT_INPUT(port->flags)) {
			if (WIP_IS_PORT_CONTROL(port->flags)) {
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
			} else {
				midiport_clear(port);
				struct pw_buffer *b = NULL;;
				struct spa_buffer *buf;
				struct spa_data *d;
				struct spa_pod *pod;
				struct spa_pod_control *c;
				b = pw_filter_dequeue_buffer(port->pwport);
				if (b != NULL) {
					buf = b->buffer;
					d = &buf->datas[0];
					if ((pod = spa_pod_from_data(d->data, d->maxsize, d->chunk->offset, d->chunk->size)) != NULL) {
						if (spa_pod_is_sequence(pod)) {
							int buf_offset = 0;
							SPA_POD_SEQUENCE_FOREACH((struct spa_pod_sequence*)pod, c) {
								struct midi_event ev;
								if (c->type != SPA_CONTROL_Midi) continue;
								ev.track = 0;
								ev.sec = (frame + c->offset) / (float) position->clock.rate.denom;
								ev.data = SPA_POD_BODY(&c->value);
								ev.size = SPA_POD_BODY_SIZE(&c->value);
								//if (ev.data[0] == 0xf8) continue;
								midiport_write(port, ev.data, ev.size , ev.sec, &buf_offset);
							}
							pw_filter_queue_buffer(port->pwport, b);
						} else pw_log_error("Pod is not sequence port %d",port->index);
					} else pw_log_error("No pod for port %d",port->index);
				}
			}
		}
	}

	// Prepare outputs
	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		if (WIP_IS_PORT_OUTPUT(port->flags)) {
			if (WIP_IS_PORT_CONTROL(port->flags)) {
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
			        float *outp = pw_filter_get_dsp_buffer(port->pwport, n_samples);
				if (outp == NULL) {
					pw_log_error("No buffer available for output port %d",port->index);
					continue;
				}
       				data->desc->connect_port(data->instance, port->index, outp);

			} else {
				LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence *)port->atomBuffer;
    				aseq->atom.size = ATOM_BUFFER_SIZE - sizeof(LV2_Atom);
    				aseq->atom.type = atomChunkUri();
			}
		}
	}

	// Run the plugin instance
	data->desc->run(data->instance, n_samples);

	// Process output ports
	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		if (WIP_IS_PORT_OUTPUT(port->flags)) {
			if (WIP_IS_PORT_CONTROL(port->flags)) {
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
			} else {
				LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence *)port->atomBuffer;
				LV2_Atom_Event* aev = (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq) );
				if (aseq->atom.size > sizeof(LV2_Atom_Sequence) ) {
					struct pw_buffer *buf;
					struct spa_data *d;
					struct spa_pod_builder builder;
					struct spa_pod_frame frame;
					if ((buf = pw_filter_dequeue_buffer(port->pwport)) == NULL) {
						pw_log_error("No pw buffer available for port %d",port->index);
						break;
					}
					spa_assert(buf->buffer->n_datas == 1);
					d = &buf->buffer->datas[0];
					d->chunk->offset = 0;
					d->chunk->size = 0;
					d->chunk->stride = 1;
					d->chunk->flags = 0;
					spa_pod_builder_init(&builder, d->data, d->maxsize);
					spa_pod_builder_push_sequence(&builder, &frame, 0);
					long payloadSize = aseq->atom.size;
					while(payloadSize > (long)sizeof(LV2_Atom_Event)) {
						if (aev->body.type == midiEventUri()) {
							uint8_t *mididata = (uint8_t *)aev + sizeof(LV2_Atom_Event);
							spa_pod_builder_control(&builder, 0, SPA_CONTROL_Midi);
							spa_pod_builder_bytes(&builder, mididata, aev->body.size);
						}
						int eventSize = lv2_atom_pad_size(sizeof(LV2_Atom_Event)) + lv2_atom_pad_size(aev->body.size);
						char *next = ((char *)aev) + eventSize;
						payloadSize = payloadSize - eventSize;
						aev = (LV2_Atom_Event *) next;
					}
					spa_pod_builder_pop(&builder, &frame);
					d->chunk->size = builder.state.offset;
					pw_filter_queue_buffer(port->pwport, buf);
				}
			}
		}
	}

}

static void on_destroy(void *data){pw_log_debug("wip on destroy");}

static void on_state_changed(void *data, enum pw_filter_state old, enum pw_filter_state state, const char *error){pw_log_debug("wip on state changed");}

static void on_io_changed(void *data, void *port_data, uint32_t id, void *area, uint32_t size){pw_log_debug("wip on io changed");}

static void on_param_changed(void *data, void *port_data, uint32_t id, const struct spa_pod *param){pw_log_debug("wip on param changed");}

static void on_add_buffer(void *data, void *port_data, struct pw_buffer *buffer){pw_log_debug("wip on add buffer");}

static void on_remove_buffer(void *data, void *port_data, struct pw_buffer *buffer){pw_log_debug("wip on remove buffer");}

static void on_drained(void *data){pw_log_debug("wip on drained");}

static void on_command(void *data, const struct spa_command *command){pw_log_debug("wip on command");}




static const struct pw_filter_events filter_events = {
	PW_VERSION_FILTER_EVENTS,
	.process = on_process,
	.destroy = on_destroy,
	.state_changed = on_state_changed,
	.io_changed = on_io_changed,
	.param_changed = on_param_changed,
	.add_buffer = on_add_buffer,
	.remove_buffer = on_remove_buffer,
	.drained = on_drained,
	.command = on_command,
};



static void do_quit(void *userdata, int signal_number)
{
	struct data *data = userdata;
	pw_main_loop_quit(data->loop);
}

int main(int argc, char *argv[])
{
	struct data data = { 0, };
	uint8_t buffer[1024];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	pw_init(&argc, &argv);

	data.loop = pw_main_loop_new(NULL);

	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, do_quit, &data);
	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, do_quit, &data);

	data.plugin = wip_plugin_load("http://gareus.org/oss/lv2/b_synth");
//	data.plugin = wip_plugin_load("http://lv2plug.in/plugins/eg-fifths");
	data.desc = data.plugin->make_desc(data.plugin,NULL);
	data.instance = data.desc->instantiate(data.desc,48000,0,NULL);

	data.filter = pw_filter_new_simple(
			pw_main_loop_get_loop(data.loop),
			data.desc->name,
			pw_properties_new(
				PW_KEY_MEDIA_TYPE, "Audio",
				PW_KEY_MEDIA_CATEGORY, "Filter",
				PW_KEY_MEDIA_ROLE, "DSP",
				NULL),
			&filter_events,
			&data);

	// Create pipewire ports corresponding to plugin ports
	for(int i = 0; i < data.desc->n_ports; i++) {
		struct wip_port *port = &data.desc->ports[i];

		if (WIP_IS_PORT_CONTROL(port->flags)) {
			if (WIP_IS_PORT_INPUT(port->flags)) {
			} else if (WIP_IS_PORT_OUTPUT(port->flags)) {
			} else {
			}
		} else if (WIP_IS_PORT_AUDIO(port->flags)) {
			if (WIP_IS_PORT_INPUT(port->flags)) {
				port->pwport = pw_filter_add_port(data.filter,
					PW_DIRECTION_INPUT,
					PW_FILTER_PORT_FLAG_MAP_BUFFERS,
					sizeof(struct port),
					pw_properties_new(
						PW_KEY_FORMAT_DSP, "32 bit float mono audio",
						PW_KEY_PORT_NAME, port->name,
					NULL),
				NULL, 0);
			} else if (WIP_IS_PORT_OUTPUT(port->flags)) {
				port->pwport = pw_filter_add_port(data.filter,
					PW_DIRECTION_OUTPUT,
					PW_FILTER_PORT_FLAG_MAP_BUFFERS,
					sizeof(struct port),
					pw_properties_new(
						PW_KEY_FORMAT_DSP, "32 bit float mono audio",
						PW_KEY_PORT_NAME, port->name,
						NULL),
					NULL, 0);
			} else {
			}
		} else {
			if (WIP_IS_PORT_INPUT(port->flags)) {
				port->pwport = pw_filter_add_port(data.filter,
                        		PW_DIRECTION_INPUT,
                        		PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                        		sizeof(struct port),
                        		pw_properties_new(
                                		PW_KEY_FORMAT_DSP, "8 bit raw midi",
                                		PW_KEY_PORT_NAME, port->name,
                                		NULL),
                        		NULL, 0);
			} else if (WIP_IS_PORT_OUTPUT(port->flags)) {
				port->pwport = pw_filter_add_port(data.filter,
                        		PW_DIRECTION_OUTPUT,
                        		PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                        		sizeof(struct port),
                        		pw_properties_new(
                                		PW_KEY_FORMAT_DSP, "8 bit raw midi",
                                		PW_KEY_PORT_NAME, port->name,
                                		NULL),
                        		NULL, 0);
			} else {
			}
		}


	}

	const struct spa_pod *params[1];
	params[0] = spa_process_latency_build(&b,
			SPA_PARAM_ProcessLatency,
			&SPA_PROCESS_LATENCY_INFO_INIT(
				.ns = 10 * SPA_NSEC_PER_MSEC
			));

	if (pw_filter_connect(data.filter,
				PW_FILTER_FLAG_RT_PROCESS,
				params, 1) < 0) {
		fprintf(stderr, "can't connect\n");
		return -1;
	}

	pw_main_loop_run(data.loop);

	pw_filter_destroy(data.filter);
	pw_main_loop_destroy(data.loop);
	pw_deinit();

	return 0;
}


