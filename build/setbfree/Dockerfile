FROM sqimage-build-platform AS build

RUN apt install -y setbfree

FROM scratch
COPY --from=build /usr/lib/lv2/b_synth /usr/lib/lv2/b_synth

