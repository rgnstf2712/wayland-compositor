#include <dbus/dbus.h>
#include <libudev.h>
#include <stdint.h>

struct session {
	struct udev *udev;
	DBusConnection *dbus;
	char *session_object_path;
};

struct session *session_create();
int session_take_device(struct session *session, uint32_t major, uint32_t minor,
int *inactive);
void session_destroy(struct session *session);
