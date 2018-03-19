#include <stdio.h>
#include <stdlib.h>
#include <wayland-server.h>

#include "session.h"
#include "drm.h"

int main()
{
	struct session *session = session_create();

	struct wl_display *display;
	display = wl_display_create();

	struct drm *drm = drm_create(display, session);
	const char *s = wl_display_add_socket_auto(display);
	printf("Created Wayland display on socket '%s'\n", s);

	wl_display_run(display);

	wl_display_destroy(display);
	printf("Destroyed Wayland display\n");

	drm_destroy(drm);
	session_destroy(session);
	return EXIT_SUCCESS;
}
