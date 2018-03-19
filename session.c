#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "session.h"

int session_get_session_by_PID(struct session *session);
int session_take_control(struct session *session);

struct session *session_create()
{
	struct session *session = malloc(sizeof(struct session));

	session->udev = udev_new();
	if (!session->udev) {
		fprintf(stderr, "udev_new failed\n");
		return NULL;
	}

	session->dbus = dbus_bus_get_private(DBUS_BUS_SYSTEM, NULL);
	dbus_connection_set_exit_on_disconnect(session->dbus, FALSE);

	session_get_session_by_PID(session);
	session_take_control(session);

	return session;
}

int session_get_session_by_PID(struct session *session)
{
	DBusMessage *req = dbus_message_new_method_call(
	"org.freedesktop.login1", "/org/freedesktop/login1",
	"org.freedesktop.login1.Manager", "GetSessionByPID");

	dbus_uint32_t pid = getpid();
	dbus_message_append_args(req,
	DBUS_TYPE_UINT32, &pid,
	DBUS_TYPE_INVALID);

	DBusError err;
	dbus_error_init(&err);
	DBusMessage *rep = dbus_connection_send_with_reply_and_block(
	session->dbus, req, -1, &err);
	dbus_message_unref(req);
	if (!rep) {
		fprintf(stderr, "%s\n", err.message);
		dbus_error_free(&err);
		return -1;
	}

	const char *session_object_path;
	dbus_message_get_args(rep, NULL,
	DBUS_TYPE_OBJECT_PATH, &session_object_path
	);
	session->session_object_path =
	malloc((strlen(session_object_path)+1)*sizeof(char));
	strcpy(session->session_object_path, session_object_path);
	dbus_message_unref(rep);
	return 0;
}

int session_take_control(struct session *session)
{
	DBusMessage *req = dbus_message_new_method_call(
	"org.freedesktop.login1", session->session_object_path,
	"org.freedesktop.login1.Session", "TakeControl");

	dbus_bool_t force = FALSE;
	dbus_message_append_args(req,
	DBUS_TYPE_BOOLEAN, &force,
	DBUS_TYPE_INVALID);

	DBusError err;
	dbus_error_init(&err);
	DBusMessage *rep = dbus_connection_send_with_reply_and_block(
	session->dbus, req, -1, &err);
	dbus_message_unref(req);
	if (!rep) {
		fprintf(stderr, "%s\n", err.message);
		dbus_error_free(&err);
		return -1;
	}

	dbus_message_unref(rep);
	return 0;
}

int session_take_device(struct session *session, uint32_t major, uint32_t minor, 
int *inactive)
{
	DBusMessage *req = dbus_message_new_method_call(
	"org.freedesktop.login1", session->session_object_path,
	"org.freedesktop.login1.Session", "TakeDevice");
	dbus_message_append_args(req,
	DBUS_TYPE_UINT32, &major,
	DBUS_TYPE_UINT32, &minor,
	DBUS_TYPE_INVALID);

	DBusError err;
	dbus_error_init(&err);
	DBusMessage *rep = dbus_connection_send_with_reply_and_block(
	session->dbus, req, -1, &err);
	dbus_message_unref(req);
	if (!rep) {
		fprintf(stderr, "%s\n", err.message);
		dbus_error_free(&err);
		return -1;
	}

	int fd;
	dbus_message_get_args(rep, NULL,
	DBUS_TYPE_UNIX_FD, &fd,
	DBUS_TYPE_BOOLEAN, inactive
	);
	dbus_message_unref(rep);
	return fd;
}

void session_destroy(struct session *session)
{
	dbus_connection_close(session->dbus);
	dbus_connection_unref(session->dbus);
	free(session->session_object_path);
	free(session);
}
