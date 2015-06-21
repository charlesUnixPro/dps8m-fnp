mkdir BIN
gcc -std=c99 \
 -U__STRICT_ANSI__  \
 -O0 -g \
 -finline-functions -fgcse-after-reload -fpredictive-commoning -fipa-cp-clone \
 -fno-unsafe-loop-optimizations -fno-strict-overflow -Wno-unused-result  \
 -DSIM_COMPILER="GCC Version: 4.8.3" \
 -I . \
 -D_GNU_SOURCE \
 -DUSE_READER_THREAD \
 -DSIM_ASYNCH_IO  \
 -DHAVE_PCREPOSIX_H \
 -DHAVE_DLOPEN=so \
 -DHAVE_GLOB \
 -DHAVE_LIBSDL \
 -I/usr/include/SDL/  \
 fnp/fnp.c \
 fnp/fnp_2.c \
 fnp/fnp_utils.c \
 fnp/fnp_mux.c \
 fnp/fnp_clk.c \
 fnp/fnp_tt.c \
 fnp/fnp_1.c \
 fnp/fnp_ipc.c \
 fnp/fnp_cmds.c \
 scp.c \
 sim_console.c \
 sim_fio.c \
 sim_timer.c \
 sim_sock.c \
 sim_tmxr.c \
 sim_ether.c \
 sim_tape.c \
 sim_disk.c \
 sim_serial.c \
 sim_video.c \
 sim_imd.c \
 -DVM_FNP \
 -DUSE_INT64 \
 -I fnp  \
 -o BIN/fnp \
 -lm -lrt -lpthread -lpcreposix -L/usr/local/lib -ldl -lSDL   \
 -lzmq -lczmq -lzyre \
 -Wl,-rpath=/usr/local/lib/

gcc -std=c99 \
 -U__STRICT_ANSI__  \
 -O0 -g \
 -finline-functions -fgcse-after-reload -fpredictive-commoning -fipa-cp-clone \
 -fno-unsafe-loop-optimizations -fno-strict-overflow -Wno-unused-result  \
 -DSIM_COMPILER="GCC Version: 4.8.3" \
 -I . \
 -D_GNU_SOURCE \
 -DUSE_READER_THREAD \
 -DSIM_ASYNCH_IO  \
 -DHAVE_PCREPOSIX_H \
 -DHAVE_DLOPEN=so \
 -DHAVE_GLOB \
 -DHAVE_LIBSDL \
 -I/usr/include/SDL/  \
 fnp/watcher.c \
 -DVM_FNP \
 -DUSE_INT64 \
 -I fnp  \
 -o BIN/watcher \
 -lm -lrt -lpthread -lpcreposix -L/usr/local/lib -ldl -lSDL   \
 -lzmq -lczmq -lzyre \
 -Wl,-rpath=/usr/local/lib/
