gexiv2 Installation
===================

To build and install gexiv2, type the following commands:

	$ meson setup build && cd build
	$ ninja
	$ ninja test
	$ ninja install

* By default, gexiv2 will install under /usr/local.  The configure script
  can customize the prefix directory.  Run `meson setup --help` for
  instructions and other installation options.
* You may need to execute `ninja install` as root (i.e. `sudo ninja install`)
  if installing to system directories.


Copyright 2016 Software Freedom Conservancy Inc.
