# astro

Ce programme est écrit C/C++ et utilise un gestionnaire de fenêtre OpenGL (libwmcgl).
Il en résulte une très grande fluidité.

Ce programme pilote une monture équatoriale du téléscope Perl Bellatrix 200/1000
Il communique avec une carte arduino pilotant les moteurs pas à pas.
Il communique via une connection TCP/IP localhost avec le logiciel de catographie stéllaire "Stellarium" modifié.
Il décode les images d"une caméra moté sur le chercheur en vu d'un asservissement.

![alt text](https://github.com/Runsys16/astro/blob/main/images/capture0.png?raw=true)



$ldd astro 

	linux-vdso.so.1 (0x00007ffccd09c000)
	libbluetooth.so.3 => /usr/lib/x86_64-linux-gnu/libbluetooth.so.3 (0x00007f6cdf5dd000)
	libX11.so.6 => /usr/lib/x86_64-linux-gnu/libX11.so.6 (0x00007f6cdf4a0000)
	libXrandr.so.2 => /usr/lib/x86_64-linux-gnu/libXrandr.so.2 (0x00007f6cdf493000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f6cdf470000)
	libGL.so.1 => /usr/lib/x86_64-linux-gnu/libGL.so.1 (0x00007f6cdf3e8000)
	libglut.so.3 => /usr/lib/x86_64-linux-gnu/libglut.so.3 (0x00007f6cdf19f000)
	libGLEW.so.2.1 => /usr/lib/x86_64-linux-gnu/libGLEW.so.2.1 (0x00007f6cdf0f3000)
	libwmcgl.so => /usr/lib/libwmcgl.so (0x00007f6cdf0a0000)
	libjpeg.so.8 => /usr/lib/x86_64-linux-gnu/libjpeg.so.8 (0x00007f6cdf01b000)
	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f6cdee39000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f6cdecea000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f6cdeccf000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f6cdeadb000)
	libxcb.so.1 => /usr/lib/x86_64-linux-gnu/libxcb.so.1 (0x00007f6cdeab1000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f6cdeaab000)
	libXext.so.6 => /usr/lib/x86_64-linux-gnu/libXext.so.6 (0x00007f6cdea96000)
	libXrender.so.1 => /usr/lib/x86_64-linux-gnu/libXrender.so.1 (0x00007f6cde88c000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f6cdf6db000)
	libGLdispatch.so.0 => /usr/lib/x86_64-linux-gnu/libGLdispatch.so.0 (0x00007f6cde7d4000)
	libGLX.so.0 => /usr/lib/x86_64-linux-gnu/libGLX.so.0 (0x00007f6cde79e000)
	libXi.so.6 => /usr/lib/x86_64-linux-gnu/libXi.so.6 (0x00007f6cde78c000)
	libXxf86vm.so.1 => /usr/lib/x86_64-linux-gnu/libXxf86vm.so.1 (0x00007f6cde785000)
	libfreetype.so.6 => /usr/lib/x86_64-linux-gnu/libfreetype.so.6 (0x00007f6cde6c6000)
	libGLU.so.1 => /usr/lib/x86_64-linux-gnu/libGLU.so.1 (0x00007f6cde654000)
	libIL.so.1 => /usr/lib/x86_64-linux-gnu/libIL.so.1 (0x00007f6cde535000)
	libXau.so.6 => /usr/lib/x86_64-linux-gnu/libXau.so.6 (0x00007f6cde52d000)
	libXdmcp.so.6 => /usr/lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007f6cde525000)
	libpng16.so.16 => /usr/lib/x86_64-linux-gnu/libpng16.so.16 (0x00007f6cde4ed000)
	libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f6cde4d1000)
	libtiff.so.5 => /usr/lib/x86_64-linux-gnu/libtiff.so.5 (0x00007f6cde450000)
	libmng.so.2 => /usr/lib/x86_64-linux-gnu/libmng.so.2 (0x00007f6cde3d3000)
	liblcms2.so.2 => /usr/lib/x86_64-linux-gnu/liblcms2.so.2 (0x00007f6cde378000)
	libbsd.so.0 => /usr/lib/x86_64-linux-gnu/libbsd.so.0 (0x00007f6cde35e000)
	libwebp.so.6 => /usr/lib/x86_64-linux-gnu/libwebp.so.6 (0x00007f6cde2f4000)
	libzstd.so.1 => /usr/lib/x86_64-linux-gnu/libzstd.so.1 (0x00007f6cde24b000)
	liblzma.so.5 => /lib/x86_64-linux-gnu/liblzma.so.5 (0x00007f6cde222000)
	libjbig.so.0 => /usr/lib/x86_64-linux-gnu/libjbig.so.0 (0x00007f6cde012000)
