#include <libudev.h>

struct drm {
	struct udev_monitor *mon;
	int gpu_fd;
};

struct drm *drm_create(struct wl_display *display, struct session *session);
void drm_destroy(struct drm *drm);
