#include <stdio.h>
#include <stdlib.h>
#include <wayland-server.h>

#include "session.h"
#include "drm.h"

static int monitor_event_fd_handler(int fd, uint32_t mask, void *data)
{
	struct drm *drm = data;
	return 0;
}

struct drm *drm_create(struct wl_display *display, struct session *session)
{
	struct drm *drm = malloc(sizeof(struct drm));

//	Start monitoring immediately so that I won't miss events
	drm->mon = udev_monitor_new_from_netlink(session->udev, "udev");
	if (!drm->mon) {
		fprintf(stderr, "udev_monitor_new_from_netlink failed\n");
		return NULL;
	}
	udev_monitor_filter_add_match_subsystem_devtype(drm->mon, "drm", NULL);
	int r = udev_monitor_enable_receiving(drm->mon);
	if (r < 0) {
		fprintf(stderr, "udev_monitor_enable_receiving failed\n");
		udev_monitor_unref(drm->mon);
		return NULL;
	}
	
	int mon_fd = udev_monitor_get_fd(drm->mon);
	struct wl_event_loop *event_loop = wl_display_get_event_loop(display);
	wl_event_loop_add_fd(event_loop, mon_fd, WL_EVENT_READABLE,
	monitor_event_fd_handler, drm);

	int inactive;
	drm->gpu_fd = session_take_device(session, 226, 0, &inactive);
	return drm;
}

void drm_destroy(struct drm *drm)
{
	udev_monitor_unref(drm->mon);
	free(drm);
}
