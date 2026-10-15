CC = cc
CFLAGS = -O2 -std=c11 -Wall -Wextra -pedantic

PREFIX = /usr/local
BINDIR = /bin

dwmstatus: dwmstatus.c
	$(CC) $(CFLAGS) $< -o $@

install: dwmstatus
	mkdir -p $(DESTDIR)$(PREFIX)/$(BINDIR)
	cp -f dwmstatus $(DESTDIR)$(PREFIX)/$(BINDIR)
	chmod 755 $(DESTDIR)$(PREFIX)/$(BINDIR)/dwmstatus

.PHONY: clean
clean:
	$(RM) -f dwmstatus
