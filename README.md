# astro

Ce programme est écrit C/C++ et utilise un gestionnaire de fenêtre OpenGL ![lib-wmcgl](https://github.com/Runsys16/lib-wmcgl).
Il en résulte une très grande fluidité.

Ce programme pilote une monture équatoriale du téléscope Perl Bellatrix 200/1000
Il communique avec une carte arduino pilotant les moteurs pas à pas.
Il communique via une connection TCP/IP localhost avec le logiciel de cartographie stéllaire "Stellarium" modifié.
Il décode les images d'une caméra montée sur le chercheur en vue d'un asservissement.

![alt text](https://github.com/Runsys16/astro/blob/main/images/capture0.png?raw=true)



$ldd astro 

	linux-vdso.so.1 (0x00007ffefe5aa000)
	libbluetooth.so.3 => /lib/x86_64-linux-gnu/libbluetooth.so.3 (0x00007fb7fd872000)
	libX11.so.6 => /lib/x86_64-linux-gnu/libX11.so.6 (0x00007fb7fd732000)
	libXrandr.so.2 => /lib/x86_64-linux-gnu/libXrandr.so.2 (0x00007fb7fd725000)
	libGL.so.1 => /lib/x86_64-linux-gnu/libGL.so.1 (0x00007fb7fd69e000)
	libglut.so.3 => /lib/x86_64-linux-gnu/libglut.so.3 (0x00007fb7fd651000)
	libGLEW.so.2.2 => /lib/x86_64-linux-gnu/libGLEW.so.2.2 (0x00007fb7fd591000)
	libwmcgl.so => /lib/libwmcgl.so (0x00007fb7fd535000)
	libjpeg.so.8 => /lib/x86_64-linux-gnu/libjpeg.so.8 (0x00007fb7fd4b4000)
	libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fb7fd288000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fb7fd1a1000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fb7fd181000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fb7fcf56000)
	libxcb.so.1 => /lib/x86_64-linux-gnu/libxcb.so.1 (0x00007fb7fcf2c000)
	libXext.so.6 => /lib/x86_64-linux-gnu/libXext.so.6 (0x00007fb7fcf17000)
	libXrender.so.1 => /lib/x86_64-linux-gnu/libXrender.so.1 (0x00007fb7fcf0a000)
	libGLdispatch.so.0 => /lib/x86_64-linux-gnu/libGLdispatch.so.0 (0x00007fb7fce52000)
	libGLX.so.0 => /lib/x86_64-linux-gnu/libGLX.so.0 (0x00007fb7fce1e000)
	libXi.so.6 => /lib/x86_64-linux-gnu/libXi.so.6 (0x00007fb7fce08000)
	libXxf86vm.so.1 => /lib/x86_64-linux-gnu/libXxf86vm.so.1 (0x00007fb7fce01000)
	libfreetype.so.6 => /lib/x86_64-linux-gnu/libfreetype.so.6 (0x00007fb7fcd39000)
	libIL.so.1 => /lib/x86_64-linux-gnu/libIL.so.1 (0x00007fb7fcc1a000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fb7fd9ad000)
	libXau.so.6 => /lib/x86_64-linux-gnu/libXau.so.6 (0x00007fb7fcc14000)
	libXdmcp.so.6 => /lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007fb7fcc0a000)
	libpng16.so.16 => /lib/x86_64-linux-gnu/libpng16.so.16 (0x00007fb7fcbcf000)
	libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007fb7fcbb3000)
	libbrotlidec.so.1 => /lib/x86_64-linux-gnu/libbrotlidec.so.1 (0x00007fb7fcba5000)
	libtiff.so.5 => /lib/x86_64-linux-gnu/libtiff.so.5 (0x00007fb7fcb1c000)
	libmng.so.2 => /lib/x86_64-linux-gnu/libmng.so.2 (0x00007fb7fca9f000)
	liblcms2.so.2 => /lib/x86_64-linux-gnu/liblcms2.so.2 (0x00007fb7fca3d000)
	libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x00007fb7fca25000)
	libbrotlicommon.so.1 => /lib/x86_64-linux-gnu/libbrotlicommon.so.1 (0x00007fb7fca02000)
	libwebp.so.7 => /lib/x86_64-linux-gnu/libwebp.so.7 (0x00007fb7fc995000)
	libzstd.so.1 => /lib/x86_64-linux-gnu/libzstd.so.1 (0x00007fb7fc8c4000)
	liblzma.so.5 => /lib/x86_64-linux-gnu/liblzma.so.5 (0x00007fb7fc899000)
	libjbig.so.0 => /lib/x86_64-linux-gnu/libjbig.so.0 (0x00007fb7fc888000)
	libdeflate.so.0 => /lib/x86_64-linux-gnu/libdeflate.so.0 (0x00007fb7fc864000)
	libmd.so.0 => /lib/x86_64-linux-gnu/libmd.so.0 (0x00007fb7fc857000)

