			{
				//void *in_buf = jack_port_get_buffer(source->sys_port, nsamples);
				void *seq_in = source->buf;

				LV2_Atom_Forge *forge = &handle->forge;
				LV2_Atom_Forge_Frame frame;
				lv2_atom_forge_set_buffer(forge, seq_in, SEQ_SIZE);
				LV2_Atom_Forge_Ref ref = lv2_atom_forge_sequence_head(forge, &frame, 0);

				//if(ref && trans_changed)
				//	ref = _trans_event(handle, forge, rolling, &pos);

				//const int n = jack_midi_get_event_count(in_buf);
				//for(int i=0; i<n; i++)
				//{
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

			}
