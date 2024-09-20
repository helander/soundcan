FROM sqimage-build-platform AS build

#RUN apt install -y lv2-dev
ADD src src/
ADD ttl ttl/
ADD Makefile .

RUN make
RUN make install

FROM scratch
COPY --from=build /usr/lib/lv2 /usr/lib/lv2
