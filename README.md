# OpenFitnessPal Search

Search backend for [OpenFitnessPal](https://git.swurl.xyz/swirl/OpenFitnessPal.git).

## Selfhosting

Note: all commands here are done as root.

## Building & Installing
To build this project, you'll need a libc implementation (only tested with glibc), optionally a separate libcrypt implementation, and Git. Most Linux distributions should have all of these by default, but in case yours doesn't:
- `pacman -S glibc libxcrypt git`
- `emerge --ask sys-libs/glibc dev-vcs/git`
- `apt install glibc git`

1. Clone this repository:

```bash
git clone https://git.swurl.xyz/swirl/OFPSearch && cd OFPSearch
```

2. Now, you need to compile.
```bash
make
```

3. Install python-myfitnesspal through pip. This may change depending on your distribution, but generally, `pip install --break-system-packages` may be the only real way.

Alternatively, modify `doc/runSearch.sh` to use a virtualenv.

Furthermore, you will need to be logged in on a browser to MyFitnessPal.

4. Now, you need to install. NGINX and systemd files are provided in this project; you may choose not to install them.

For all install commands, you may optionally provide `prefix` and `DESTDIR` options. This is useful for packagers; i.e. for a PKGBUILD: `make prefix=/usr DESTDIR=${pkgdir} install`.

Available install commands are as follows:
- `make install` installs the executable, NGINX, and systemd files.
- `make install-bin` installs the executable file.
- `make install-systemd` installs the systemd file, as well as its environment file.
- `make install-nginx` installs the NGINX file.

For example, on a non-systemd system using NGINX, you would run `make install-bin install-nginx`.

5. If using systemd, change the environment file to reflect your desired options:
```bash
vim /etc/search.conf
```

6. You can now enable and start the service:
```bash
systemctl enable --now search
```

The server should now be running on localhost at port 8888.

## NGINX Reverse Proxy
An NGINX file is provided with this project.

For this, you'll need [NGINX](https://nginx.org/en/download.html) (obviously), certbot, and its NGINX plugin. Most Linux distributions should have these in their repositories, i.e.:
- `pacman -S nginx certbot-nginx`
- `emerge --ask www-servers/nginx app-crypt/certbot-nginx`
- `apt install nginx python-certbot-nginx`

This section assumes you've already followed the last.

1. Change the domain in the NGINX file:
```bash
sed -i 's/your.doma.in/[DOMAIN HERE]' /etc/nginx/sites-available/search
```

2. Enable the site:
```bash
ln -s /etc/nginx/sites-{available,enabled}/search
```

3. Enable HTTPS for the site:
```bash
certbot --nginx -d [DOMAIN HERE]
```

4. Enable and start NGINX:
```bash
systemctl enable --now nginx
```

If it's already running, reload:
```bash
systemctl reload nginx
```

Your site should be running at https://your.doma.in.

# Contributions
Contributions are always welcome.
