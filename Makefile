include config.mk

dwmstatus: dwmstatus.c
	$(CC) $(CFLAGS) $(LIBS) $< -o $@

install: dwmstatus
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dwmstatus $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dwmstatus

.PHONY: clean
clean:
	$(RM) -f dwmstatus
