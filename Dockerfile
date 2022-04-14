FROM debian:bullseye AS builder

RUN apt update
RUN apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev \
    libevent-dev bsdmainutils libgmp-dev cmake curl software-properties-common

# Install Berkeley DB 4.8
RUN curl -L http://download.oracle.com/berkeley-db/db-4.8.30.tar.gz | tar -xz -C /tmp && \
    cd /tmp/db-4.8.30/build_unix && \
    ../dist/configure --enable-cxx --includedir=/usr/include/bdb4.8 --libdir=/usr/lib && \
    sed -i 's/__atomic_compare_exchange/__atomic_compare_exchange_db/g' /tmp/db-4.8.30/dbinc/atomic.h && \
    make -j$(nproc) && make install && \
    cd / && rm -rf /tmp/db-4.8.30

# Install zlib
RUN curl -L https://www.zlib.net/zlib-1.2.12.tar.gz | tar -xz -C /tmp && \
    cd /tmp/zlib-1.2.12/contrib/minizip && \
    autoreconf -fi && \
    ./configure --enable-shared=no --with-pic && \
    make -j$(nproc) install && \
    cd / && rm -rf /tmp/zlib-1.2.12

# Install zmq from source (outdated version from apt on Ubuntu 14.04)
RUN curl -L https://github.com/zeromq/libzmq/releases/download/v4.3.1/zeromq-4.3.1.tar.gz | tar -xz -C /tmp && \
    cd /tmp/zeromq-4.3.1/ && ./configure --disable-shared --without-libsodium --with-pic && \
    make -j$(nproc) install && \
    cd / && rm -rf /tmp/zeromq-4.3.1/

# Install lib boost
RUN curl -L https://boostorg.jfrog.io/artifactory/main/release/1.64.0/source/boost_1_64_0.tar.gz | tar -xz -C /tmp && \
    cd /tmp/boost_1_64_0/ && \
    ./bootstrap.sh --prefix=/usr && \
    ./bjam -j$(nproc) install

WORKDIR /work
COPY . .

RUN ./autogen.sh && \
    ./configure --disable-tests --disable-bench --without-gui --prefix=`pwd`/depends/x86_64-pc-linux-gnu \
    LDFLAGS="-static-libstdc++ -static -L/usr/lib/x86_64-linux-gnu" \
    CXXFLAGS="-static" && \
    make clean && \
    make -j$(nproc)

RUN strip /work/src/firovmd /work/src/firovm-cli

FROM debian:bullseye

RUN apt-get update
RUN apt-get install -y libevent-dev libgmp-dev

RUN useradd -ms /bin/bash firovm
USER firovm

RUN mkdir -p /home/firovm/.fvm/
RUN chown firovm:firovm /home/firovm/.fvm/

COPY --from=builder /work/fvm.regtest.conf /home/firovm/.fvm/fvm.conf
COPY --from=builder /work/src/firovmd /usr/local/bin
COPY --from=builder /work/src/firovm-cli /usr/local/bin
COPY --from=builder /usr/lib/libboost_system.so.1.64.0 \
     /usr/lib/libboost_filesystem.so.1.64.0 \
     /usr/lib/libboost_thread.so.1.64.0 \
     /usr/lib/libboost_program_options.so.1.64.0 \
     /usr/lib/libboost_thread.so.1.64.0 \
     /usr/lib/libdb_cxx-4.8.so \
     /usr/lib/

CMD [ "firovmd", "--daemon=0", "--printtoconsole", "--rpcbind=0.0.0.0", "--rpcallowip=0.0.0.0/0" ]