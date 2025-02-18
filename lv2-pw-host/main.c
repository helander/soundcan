/* PipeWire */
/* SPDX-FileCopyrightText: Copyright © 2019 Wim Taymans */
/* SPDX-License-Identifier: MIT */

/*
 [title]
 Audio filter using \ref pw_filter "pw_filter".
 [title]
 */

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
//	struct port *in_left_port;
//	struct port *out_left_port;
//	struct port *in_right_port;
//	struct port *out_right_port;
//	struct port *midi_in_port;
//	struct port *midi_out_port;
	int64_t clock_time;
	struct wip_plugin *plugin;
	struct wip_descriptor *desc;
	void *instance;
};

/* our data processing function is in general:
 *
 *  struct pw_buffer *b;
 *  in = pw_filter_dequeue_buffer(filter, in_port);
 *  out = pw_filter_dequeue_buffer(filter, out_port);
 *
 *  .. do stuff with buffers ...
 *
 *  pw_filter_queue_buffer(filter, in_port, in);
 *  pw_filter_queue_buffer(filter, out_port, out);
 *
 *  For DSP ports, there is a shortcut to directly dequeue, get
 *  the data and requeue the buffer with pw_filter_get_dsp_buffer().
 *
 *
 */
static void on_process(void *userdata, struct spa_io_position *position)
{
	struct data *data = userdata;
//	struct pw_buffer *inL, *inR;
	struct pw_buffer *outL = NULL, *outR = NULL;
//	uint32_t n_samples = position->clock.duration;
	int32_t stride = 0;
//	pw_log_trace("do process %d", n_samples);


	uint32_t outsize = 0;

//	struct pw_buffer *b = NULL;;
//	struct spa_buffer *buf;
//	struct spa_data *d;
//	struct spa_pod *pod;
//	struct spa_pod_control *c;
	uint64_t frame;

	frame = data->clock_time;
	data->clock_time += position->clock.duration;


	pw_log_info("wip on process intro");

	//Process inputs

	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		pw_log_info("wip on process port #%d",i);
		if (WIP_IS_PORT_INPUT(port->flags)) {
			if (WIP_IS_PORT_CONTROL(port->flags)) {
				pw_log_info("wip on process control port ");
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
				pw_log_info("wip on process audio port ");
			} else {
				pw_log_info("wip on process atom port ");
				midiport_clear(port);
				pw_log_info("wip on process input port ");
				struct pw_buffer *b = NULL;;
				struct spa_buffer *buf;
				struct spa_data *d;
				struct spa_pod *pod;
				struct spa_pod_control *c;

				pw_log_info("wip on process midi input port entry %d",0);
				b = pw_filter_dequeue_buffer(port->pwport);
				pw_log_info("wip on process midi input port entry %d",1);
				if (b != NULL) {
					pw_log_info("wip on process midi input port entry %d",2);

					buf = b->buffer;
					pw_log_info("wip on process midi input port entry %d",3);
					d = &buf->datas[0];

					pw_log_info("wip on process midi input port entry %d",4);

					if ((pod = spa_pod_from_data(d->data, d->maxsize, d->chunk->offset, d->chunk->size)) == NULL)
						goto xdone;
					if (!spa_pod_is_sequence(pod))
						goto xdone;
					int buf_offset = 0;
					SPA_POD_SEQUENCE_FOREACH((struct spa_pod_sequence*)pod, c) {
						struct midi_event ev;
						pw_log_info("wip on process midi input port sequence entry");

						if (c->type != SPA_CONTROL_Midi) continue;

						ev.track = 0;
						ev.sec = (frame + c->offset) / (float) position->clock.rate.denom;
						ev.data = SPA_POD_BODY(&c->value);
						ev.size = SPA_POD_BODY_SIZE(&c->value);
						if (ev.data[0] == 0xf8) continue;

						printf("\n%04d: %d %x", c->offset,ev.size,ev.data[0]);
						midiport_write(port, ev.data, ev.size , ev.sec, &buf_offset);
						pw_log_info("wip on process midi input port sequence exit");


					}
					pw_filter_queue_buffer(port->pwport, b);

				}
xdone:
			}
		}


	}


	// Prepare outputs

	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		pw_log_info("wip on process port #%d",i);
		if (WIP_IS_PORT_OUTPUT(port->flags)) {
			if (WIP_IS_PORT_CONTROL(port->flags)) {
				//pw_log_info("wip on process control port ");
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
				//pw_log_info("wip on process audio port ");
			} else {
				//pw_log_info("wip on process atom port ");
				LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence *)port->atomBuffer;
    				aseq->atom.size = 1024;
    				aseq->atom.type = atomChunkUri();
			}
		}
	}

	pw_log_info("wip on process ports done");
	data->desc->run(data->instance, outsize / sizeof(float));
	pw_log_info("wip on process run done");



	// Process output ports
	for(int i = 0; i < data->desc->n_ports; i++) {
		struct wip_port *port = &data->desc->ports[i];
		if (WIP_IS_PORT_OUTPUT(port->flags)) {
			pw_log_info("wip on process output port #%d ",i);
			if (WIP_IS_PORT_CONTROL(port->flags)) {
				pw_log_info("wip on process control port ");
			} else if (WIP_IS_PORT_AUDIO(port->flags)) {
				pw_log_info("wip on process audio port ");
			} else {
				pw_log_info("wip on process atom port ");
				LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence *)port->atomBuffer;
				LV2_Atom_Event* aev = (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq) );

				//...
				if (aseq->atom.size > sizeof(LV2_Atom_Sequence) ) {
					long payloadSize = aseq->atom.size - sizeof(LV2_Atom_Sequence);
					printf("\npayload size %d",payloadSize);fflush(stdout);
					while(payloadSize > (long)sizeof(LV2_Atom_Event)) {
						printf("\n	aev type %d    size %d",aev->body.type,aev->body.size);fflush(stdout);
						if (aev->body.type == midiEventUri()) {
							uint8_t *mididata = (uint8_t *)aev + sizeof(LV2_Atom_Event);
							printf("\nmidi event  %d   %02x",aev->body.size,mididata[0]);fflush(stdout);
						}
						int eventSize = lv2_atom_pad_size(sizeof(LV2_Atom_Event)) + lv2_atom_pad_size(aev->body.size);
						char *next = ((char *)aev) + eventSize;
						payloadSize = payloadSize - eventSize;
						aev = (LV2_Atom_Event *) next;
						//printf("\n	payload size %ld   event size %d        %d",payloadSize,eventSize,sizeof(LV2_Atom_Event));fflush(stdout);
					}
				}
/*
LV2_Atom_Sequence* aseq = (LV2_Atom_Sequence *)port->atomBuffer;
  if (ATOM_BUFFER_SIZE - sizeof(LV2_Atom) - aseq->atom.size < sizeof(LV2_Atom_Event) + midibytes) {
    return;
  }

  LV2_Atom_Event* aev = (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq) + *offset);

  aev->time.frames = evtime;
  aev->body.type   = _context->midi_MidiEvent;
  aev->body.size   = midibytes;
  memcpy(LV2_ATOM_BODY(&aev->body), mididata, midibytes);

  int size = lv2_atom_pad_size(sizeof(LV2_Atom_Event) + midibytes);
  aseq->atom.size += size;
  *offset += size;

}

*/
			}
		}


	}





	if (outsize == 0) return;





//	outL = pw_filter_dequeue_buffer(data->out_left_port);
//	outR = pw_filter_dequeue_buffer(data->out_right_port);

//	inL = pw_filter_get_dsp_buffer(data->in_left_port, n_samples);
//	outL = pw_filter_get_dsp_buffer(data->out_left_port, n_samples);
//	inR = pw_filter_get_dsp_buffer(data->in_right_port, n_samples);
//	outR = pw_filter_get_dsp_buffer(data->out_right_port, n_samples);

//	if (inL != NULL && outL != NULL) memcpy(outL, inL, n_samples * sizeof(float));
//	if (inR != NULL && outR != NULL) memcpy(outR, inR, n_samples * sizeof(float));
	if (outL != NULL) {

		struct spa_data *bd;

		//bd = &outL->buffer->datas[i]; 
		bd = &outL->buffer->datas[0]; 

		outsize = SPA_MIN(outsize, bd->maxsize);

		//port = i < graph->n_output ? &graph->output[i] : NULL;

		/*if (data->out_left_port)
			data->desc->connect_port(data->instance, data->out_left_port->pluginPort, bd->data);
		else
			memset(bd->data, 0, outsize);*/

		bd->chunk->offset = 0;
		bd->chunk->size = outsize;
		bd->chunk->stride = stride;
	

		//pw_log_trace_fp("%p: stride:%d in:%d out:%d requested:%"PRIu64" (%"PRIu64")", impl,
		//		stride, insize, outsize, out->requested, out->requested * stride);

		data->desc->run(data->instance, outsize / sizeof(float));



	}
	if (outR != NULL) {}
	pw_log_info("wip on process exit");

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
	const struct spa_pod *params[1];
	uint8_t buffer[1024];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

	pw_init(&argc, &argv);

	/* make a main loop. If you already have another main loop, you can add
	 * the fd of this pipewire mainloop to it. */
	data.loop = pw_main_loop_new(NULL);

	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, do_quit, &data);
	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, do_quit, &data);

	/* Create a simple filter, the simple filter manages the core and remote
	 * objects for you if you don't need to deal with them.
	 *
	 * Pass your events and a user_data pointer as the last arguments. This
	 * will inform you about the filter state. The most important event
	 * you need to listen to is the process event where you need to process
	 * the data.
	 */

//	data.plugin = wip_plugin_load("http://gareus.org/oss/lv2/b_synth");
	data.plugin = wip_plugin_load("http://lv2plug.in/plugins/eg-fifths");
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




	for(int i = 0; i < data.desc->n_ports; i++) {
		struct wip_port *port = &data.desc->ports[i];

		printf("\nport %d %d %s %lx input %lld output %lld control %lld audio %lld",i,port->index,port->name,port->flags,
			WIP_IS_PORT_INPUT(port->flags),
			WIP_IS_PORT_OUTPUT(port->flags),
			WIP_IS_PORT_CONTROL(port->flags),
			WIP_IS_PORT_AUDIO(port->flags)
		);
		if (WIP_IS_PORT_CONTROL(port->flags)) {
			if (WIP_IS_PORT_INPUT(port->flags)) {
				printf("\ncontrol input port");
			} else if (WIP_IS_PORT_OUTPUT(port->flags)) {
				printf("\ncontrol output port");
			} else {
				printf("\ncontrol port without direction");
			}
		} else if (WIP_IS_PORT_AUDIO(port->flags)) {
			if (WIP_IS_PORT_INPUT(port->flags)) {
				printf("\naudio input port");
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
				printf("\naudio output port");
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
				printf("\naudio port without direction");
			}
		} else {
			if (WIP_IS_PORT_INPUT(port->flags)) {
				printf("\natom input port");
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
				printf("\natom output port");
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
				printf("\natom port without direction");
			}
		}


	}

	/* Now connect this filter. We ask that our process function is
	 * called in a realtime thread. */

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



	/* and wait while we let things run */
	pw_main_loop_run(data.loop);

	pw_filter_destroy(data.filter);
	pw_main_loop_destroy(data.loop);
	pw_deinit();

	return 0;
}


