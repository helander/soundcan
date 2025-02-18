	// fill input buffers
	for(const sp_app_system_source_t *source=sources;
		source->type != SYSTEM_PORT_NONE;
		source++)
	{
		switch(source->type)
		{
			case SYSTEM_PORT_NONE:
			case SYSTEM_PORT_CONTROL:
				break;

			case SYSTEM_PORT_AUDIO:
			case SYSTEM_PORT_CV:
			{
				const void *in_buf = jack_port_get_buffer(source->sys_port, nsamples);
				memcpy(source->buf, in_buf, sample_buf_size);
				break;
			}
			case SYSTEM_PORT_MIDI:
			{
				void *in_buf = jack_port_get_buffer(source->sys_port, nsamples);
				void *seq_in = source->buf;

				LV2_Atom_Forge *forge = &handle->forge;
				LV2_Atom_Forge_Frame frame;
				lv2_atom_forge_set_buffer(forge, seq_in, SEQ_SIZE);
				LV2_Atom_Forge_Ref ref = lv2_atom_forge_sequence_head(forge, &frame, 0);

				if(ref && trans_changed)
					ref = _trans_event(handle, forge, rolling, &pos);

				const int n = jack_midi_get_event_count(in_buf);
				for(int i=0; i<n; i++)
				{
					jack_midi_event_t mev;
					jack_midi_event_get(&mev, in_buf, i);

					if( (mev.buffer[0] & 0x80) != 0x80)
						continue; // no MIDI message

					//add jack midi event to in_buf
					if(ref)
						ref = lv2_atom_forge_frame_time(forge, mev.time);
					if(ref)
						ref = lv2_atom_forge_atom(forge, mev.size, handle->midi_MidiEvent);
					// fix up noteOn(vel=0) -> noteOff(vel=0)
					if(  (mev.size == 3) && ( (mev.buffer[0] & 0xf0) == 0x90)
						&& (mev.buffer[2] == 0x00) )
					{
						const uint8_t note_off [3] = {
							0x80 | (mev.buffer[0] & 0xf),
							mev.buffer[1],
							0x0
						};
						if(ref)
							ref = lv2_atom_forge_write(forge, note_off, sizeof(note_off));
					}
					else
					{
						if(ref)
							ref = lv2_atom_forge_write(forge, mev.buffer, mev.size);
					}
				}
				if(ref)
					lv2_atom_forge_pop(forge, &frame);
				else
					lv2_atom_sequence_clear(seq_in);

				break;
			}

			case SYSTEM_PORT_OSC:
			{
				void *in_buf = jack_port_get_buffer(source->sys_port, nsamples);
				void *seq_in = source->buf;

				LV2_Atom_Forge *forge = &handle->forge;
				LV2_Atom_Forge_Frame frame;
				lv2_atom_forge_set_buffer(forge, seq_in, SEQ_SIZE);
				LV2_Atom_Forge_Ref ref = lv2_atom_forge_sequence_head(forge, &frame, 0);

				if(ref && trans_changed)
					ref = _trans_event(handle, forge, rolling, &pos);

				const int n = jack_midi_get_event_count(in_buf);
				for(int i=0; i<n; i++)
				{
					jack_midi_event_t mev;
					jack_midi_event_get(&mev, in_buf, i);

					if( (mev.buffer[0] != '/') && (mev.buffer[0] != '#') )
						continue; // no OSC message

					if(ref)
						ref = lv2_atom_forge_frame_time(forge, mev.time);
					if(ref)
						ref = lv2_osc_forge_packet(forge, &handle->osc_urid, handle->bin.map,
							mev.buffer, mev.size); // Note: an invalid packet will return 0
				}
				if(ref)
					lv2_atom_forge_pop(forge, &frame);
				else
					lv2_atom_sequence_clear(seq_in);

				break;
			}

			case SYSTEM_PORT_COM:
			{
				void *seq_in = source->buf;

				LV2_Atom_Forge *forge = &handle->forge;
				LV2_Atom_Forge_Frame frame;
				lv2_atom_forge_set_buffer(forge, seq_in, SEQ_SIZE);
				LV2_Atom_Forge_Ref ref = lv2_atom_forge_sequence_head(forge, &frame, 0);

				const LV2_Atom_Object *obj;
				size_t size;
				while((obj = varchunk_read_request(bin->app_from_com, &size)))
				{
					if(ref)
						ref = lv2_atom_forge_frame_time(forge, 0);
					if(ref)
						ref = lv2_atom_forge_write(forge, obj, size);

					varchunk_read_advance(bin->app_from_com);
				}
				if(ref)
					lv2_atom_forge_pop(forge, &frame);
				else
					lv2_atom_sequence_clear(seq_in);

				break;
			}
		}
	}

	// update transport state
	handle->trans.rolling = rolling;
	handle->trans.frame = rolling
		? handle->trans.frame + nsamples
		: pos.frame;
	handle->trans.beats_per_bar = pos.beats_per_bar;
	handle->trans.beat_type = pos.beat_type;
	handle->trans.ticks_per_beat = pos.ticks_per_beat;
	handle->trans.beats_per_minute = pos.beats_per_minute;

	bin_process_pre(bin, nsamples, false);

	const sp_app_system_sink_t *sinks = sp_app_get_system_sinks(app);

	// fill output buffers
	for(const sp_app_system_sink_t *sink=sinks;
		sink->type != SYSTEM_PORT_NONE;
		sink++)
	{
		switch(sink->type)
		{
			case SYSTEM_PORT_NONE:
			case SYSTEM_PORT_CONTROL:
				break;

			case SYSTEM_PORT_AUDIO:
			case SYSTEM_PORT_CV:
			{
				void *out_buf = jack_port_get_buffer(sink->sys_port, nsamples);
				memcpy(out_buf, sink->buf, sample_buf_size);
				break;
			}
			case SYSTEM_PORT_MIDI:
			{
				void *out_buf = jack_port_get_buffer(sink->sys_port, nsamples);
				const LV2_Atom_Sequence *seq_out = sink->buf;

				// fill midi output buffer
				jack_midi_clear_buffer(out_buf);
				if(seq_out)
				{
					LV2_ATOM_SEQUENCE_FOREACH(seq_out, ev)
					{
						const LV2_Atom *atom = &ev->body;

						if(atom->type != handle->midi_MidiEvent)
							continue; // ignore non-MIDI events

						jack_midi_event_write(out_buf, ev->time.frames,
							LV2_ATOM_BODY_CONST(atom), atom->size);
					}
				}

				break;
			}

			case SYSTEM_PORT_OSC:
			{
				void *out_buf = jack_port_get_buffer(sink->sys_port, nsamples);
				const LV2_Atom_Sequence *seq_out = sink->buf;

				// fill midi output buffer
				jack_midi_clear_buffer(out_buf);
				if(seq_out)
				{
					LV2_ATOM_SEQUENCE_FOREACH(seq_out, ev)
					{
						const LV2_Atom_Object *obj = (const LV2_Atom_Object *)&ev->body;

						if(!lv2_atom_forge_is_object_type(&handle->forge, obj->atom.type))
							continue;

						if(!lv2_osc_is_message_or_bundle_type(&handle->osc_urid, obj->body.otype))
							continue;

						LV2_OSC_Writer writer;
						lv2_osc_writer_initialize(&writer, handle->osc_buf, OSC_SIZE);
						lv2_osc_writer_packet(&writer, &handle->osc_urid, handle->bin.unmap,
							obj->atom.size, &obj->body);
						size_t size;
						uint8_t *osc_buf = lv2_osc_writer_finalize(&writer, &size);

						if(size)
						{
							jack_midi_event_write(out_buf, ev->time.frames,
								osc_buf, size);
						}
					}
				}

				break;
			}

			case SYSTEM_PORT_COM:
			{
				const LV2_Atom_Sequence *seq_out = sink->buf;

				LV2_ATOM_SEQUENCE_FOREACH(seq_out, ev)
				{
					const LV2_Atom *atom = (const LV2_Atom *)&ev->body;

					// try do process events directly
					bin->advance_ui = sp_app_from_ui(bin->app, atom);
					if(!bin->advance_ui) // queue event in ringbuffer instead
					{
						//fprintf(stderr, "plugin ui direct is blocked\n");

						void *ptr;
						size_t size = lv2_atom_total_size(atom);
						if((ptr = varchunk_write_request(bin->app_from_app, size)))
						{
							memcpy(ptr, atom, size);
							varchunk_write_advance(bin->app_from_app, size);
						}
						else
						{
							bin_log_trace(bin, "%s: app_from_app ringbuffer full\n", __func__);
							//FIXME
						}
					}
				}
				break;
			}
		}
	}

	bin_process_post(bin);

	return 0;
}

