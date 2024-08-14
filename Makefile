prefix=/usr/local
confdir=/etc
systemd_dir=${DESTDIR}${confdir}/systemd/system
nginx_dir=${DESTDIR}${confdir}/nginx
bindir=${DESTDIR}${prefix}/bin

CC			?= gcc
CFLAGS		:= -O2 -g

BIN			:= search

SOURCE	:= main.c mongoose.c
OBJ		:= mongoose.o main.o
DEPS	:= mongoose.h index.h
LIBS	:= -lcrypt

all: $(BIN)

clean:
	rm -f $(OBJ)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

install-nginx:
	@install -Dm644 doc/search.nginx ${nginx_dir}/sites-available/search

install-systemd:
	@install -Dm644 doc/search.service ${systemd_dir}/search.service
	@echo "BINDIR=${bindir}" > ${DESTDIR}/${confdir}/search.conf
	@echo "BINNAME=${BIN}" >> ${DESTDIR}/${confdir}/search.conf
	@echo "BIN=${bindir}/${BIN}" >> ${DESTDIR}/${confdir}/search.conf
	@echo "DBUSBIN=${bindir}/runSearch.sh" >> ${DESTDIR}/${confdir}/search.conf

install-bin:
	@install -Dm755 ${BIN} ${bindir}/${BIN}
	@install -Dm755 search.py ${bindir}/search.py
	@install -Dm755 doc/runSearch.sh ${bindir}/runSearch.sh

install: install-bin install-nginx install-systemd
