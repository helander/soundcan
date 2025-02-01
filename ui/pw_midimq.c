#include <stdio.h>

#include <pipewire/pipewire.h>
#include <pipewire/filter.h>

#include <spa/pod/builder.h>
#include <spa/control/control.h>

#include <mqueue.h>


#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10


struct port {
};

struct data {
	struct pw_main_loop *loop;
	struct pw_filter *filter;
	struct port *port;
};

mqd_t qd_server;

static void on_process(void *userdata, struct spa_io_position *position)
{
	struct data *data = userdata;
	struct port *port = data->port;
	struct pw_buffer *buf;
	struct spa_data *d;
	struct spa_pod_builder builder;
	struct spa_pod_frame frame;

	char msg_buffer [MSG_BUFFER_SIZE];
	int bytes = 0;

	bytes = mq_receive(qd_server, msg_buffer, MSG_BUFFER_SIZE, NULL);
	if (bytes == -1) return; // No messages availble

	if ((buf = pw_filter_dequeue_buffer(port)) == NULL)
		return;

	spa_assert(buf->buffer->n_datas == 1);

	d = &buf->buffer->datas[0];
	d->chunk->offset = 0;
	d->chunk->size = 0;
	d->chunk->stride = 1;
	d->chunk->flags = 0;

	spa_pod_builder_init(&builder, d->data, d->maxsize);
	spa_pod_builder_push_sequence(&builder, &frame, 0);

	spa_pod_builder_control(&builder, 0, SPA_CONTROL_Midi);
	spa_pod_builder_bytes(&builder, msg_buffer, bytes);

	while ((bytes = mq_receive (qd_server, msg_buffer, MSG_BUFFER_SIZE, NULL)) != -1) {
			spa_pod_builder_control(&builder, 0, SPA_CONTROL_Midi);
			spa_pod_builder_bytes(&builder, msg_buffer, bytes);
	}

        spa_pod_builder_pop(&builder, &frame);
        d->chunk->size = builder.state.offset;
	pw_filter_queue_buffer(port, buf);
}

static const struct pw_filter_events filter_events = {
	PW_VERSION_FILTER_EVENTS,
	.process = on_process,
};

static void do_quit(void *userdata, int signal_number)
{
	struct data *data = userdata;
	pw_main_loop_quit(data->loop);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("\nMissing queue name");
		exit(1);
	}

	struct data data = {};

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;
	char q_name[100];
	q_name[0] = 0;
	strcat(q_name,"/"); strcat(q_name,argv[1]);
	printf("\nQueue name: %s\n",q_name);

	if ((qd_server = mq_open (q_name, O_RDONLY | O_CREAT | O_NONBLOCK, QUEUE_PERMISSIONS, &attr)) == -1) {
		perror ("Server: mq_open (server)");
		exit (1);
	}

	pw_init(&argc, &argv);

	data.loop = pw_main_loop_new(NULL);

	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, do_quit, &data);
	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, do_quit, &data);

	data.filter = pw_filter_new_simple(
			pw_main_loop_get_loop(data.loop),
			argv[1],
			pw_properties_new(
				PW_KEY_MEDIA_TYPE, "Midi",
				PW_KEY_MEDIA_CATEGORY, "Playback",
				PW_KEY_MEDIA_CLASS, "Midi/Source",
				NULL),
			&filter_events,
			&data);

	data.port = pw_filter_add_port(data.filter,
			PW_DIRECTION_OUTPUT,
			PW_FILTER_PORT_FLAG_MAP_BUFFERS,
			sizeof(struct port),
			pw_properties_new(
				PW_KEY_FORMAT_DSP, "8 bit raw midi",
				PW_KEY_PORT_NAME, "output",
				NULL),
			NULL, 0);

	if (pw_filter_connect(data.filter, PW_FILTER_FLAG_RT_PROCESS, NULL, 0) < 0) {
		fprintf(stderr, "can't connect\n");
		return -1;
	}

	pw_main_loop_run(data.loop);

	pw_filter_destroy(data.filter);
	pw_main_loop_destroy(data.loop);
	pw_deinit();

	return 0;
}
