bool
lv2_evbuf_write(LV2_Evbuf_Iterator* iter,
                uint32_t            frames,
                uint32_t            subframes,
                uint32_t            type,
                uint32_t            size,
                const void*         data)
{
  (void)subframes;

  LV2_Atom_Sequence* aseq = &iter->evbuf->buf;
  if (iter->evbuf->capacity - sizeof(LV2_Atom) - aseq->atom.size <
      sizeof(LV2_Atom_Event) + size) {
    return false;
  }

  LV2_Atom_Event* aev =
    (LV2_Atom_Event*)((char*)LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq) +
                      iter->offset);

  aev->time.frames = frames;
  aev->body.type   = type;
  aev->body.size   = size;
  memcpy(LV2_ATOM_BODY(&aev->body), data, size);

  size = lv2_atom_pad_size(sizeof(LV2_Atom_Event) + size);
  aseq->atom.size += size;
  iter->offset += size;

  return true;
}
