/*
 * xsp -- paste X11 clipboard selection.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>

static Atom CLP;
static Atom UTF;
static Atom SEL;

/*
 * Creates a dummy window, makes a request for providing a CLIPBOARD selection
 * in UTF-8 format and puts the result into the SEL atom. If the selection data
 * was converted successfully, print it into stdout.
 */
int
main(void)
{
	Atom tp, da;
	Display *dpl;
	Window w;
	XEvent ev;
	XSelectionEvent *snev;
	int di;
	unsigned char *sel = NULL;
	unsigned long sz, dul;

	if ((dpl = XOpenDisplay(NULL)) == NULL)
		err(1, "XOpenDisplay()");
	CLP = XInternAtom(dpl, "CLIPBOARD", False);
	UTF = XInternAtom(dpl, "UTF8_STRING", False);
	SEL = XInternAtom(dpl, "SEL", False);
	w = XCreateSimpleWindow(dpl, DefaultRootWindow(dpl), 0, 0, 1, 1, 0, 0, 0);
	XConvertSelection(dpl, CLP, UTF, SEL, w, CurrentTime);

	for (;;) {
		XNextEvent(dpl, &ev);
		switch (ev.type) {
		case SelectionNotify:
			snev = &(ev.xselection);
			/*
			 * The selection owner isn't capable of converting the
			 * selection into UTF-8.
			 */
			if (snev->property == None)
				errx(1, "Can not convert selection");
			else {
				/* Make this request to obtain data size. */
				XGetWindowProperty(dpl, w, SEL, 0, 0, False,
				    AnyPropertyType, &tp, &di, &dul, &sz,  &sel);
				XGetWindowProperty(dpl, w, SEL, 0, sz, False,
				    AnyPropertyType, &da, &di, &dul, &dul, &sel);
				write(STDOUT_FILENO, sel, sz);
				exit(0);
			}
		}
	}
	return (0);
}
