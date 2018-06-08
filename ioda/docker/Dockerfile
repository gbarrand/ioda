# Copyright (C) 2010, Guy Barrand. All rights reserved.
# See the file bush.license for terms.
FROM centos
# the below will copy app/ioda in /app/ioda :
COPY app /app/
ENV LD_LIBRARY_PATH /app
#ENV env_debug_0 dummy
ENTRYPOINT /app/ioda/1.14.0/bin/ioda
COPY libpthread.so.0 /app/libpthread.so.0
COPY libdl.so.2 /app/libdl.so.2
COPY libX11.so.6 /app/libX11.so.6
COPY libGLU.so.1 /app/libGLU.so.1
COPY libGL.so.1 /app/libGL.so.1
COPY libstdc++.so.6 /app/libstdc++.so.6
COPY libm.so.6 /app/libm.so.6
COPY libgcc_s.so.1 /app/libgcc_s.so.1
COPY libc.so.6 /app/libc.so.6
COPY libxcb.so.1 /app/libxcb.so.1
COPY libexpat.so.1 /app/libexpat.so.1
COPY libxcb-dri3.so.0 /app/libxcb-dri3.so.0
COPY libxcb-xfixes.so.0 /app/libxcb-xfixes.so.0
COPY libxcb-present.so.0 /app/libxcb-present.so.0
COPY libxcb-sync.so.1 /app/libxcb-sync.so.1
COPY libxshmfence.so.1 /app/libxshmfence.so.1
COPY libglapi.so.0 /app/libglapi.so.0
COPY libselinux.so.1 /app/libselinux.so.1
COPY libXext.so.6 /app/libXext.so.6
COPY libXdamage.so.1 /app/libXdamage.so.1
COPY libXfixes.so.3 /app/libXfixes.so.3
COPY libX11-xcb.so.1 /app/libX11-xcb.so.1
COPY libxcb-glx.so.0 /app/libxcb-glx.so.0
COPY libxcb-dri2.so.0 /app/libxcb-dri2.so.0
COPY libXxf86vm.so.1 /app/libXxf86vm.so.1
COPY libdrm.so.2 /app/libdrm.so.2
COPY libXau.so.6 /app/libXau.so.6
COPY libpcre.so.1 /app/libpcre.so.1
