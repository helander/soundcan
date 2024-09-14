FROM sqimage-jalv-http

RUN apt install -y lv2-dev
ADD src src/
ADD ttl ttl/
ADD Makefile .

RUN make
RUN make install

