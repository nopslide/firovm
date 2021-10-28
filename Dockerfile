FROM ubuntu:18.04

RUN apt update
RUN apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils libboost-all-dev libgmp-dev cmake
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:bitcoin/bitcoin
RUN apt-get update
RUN apt-get -y install libdb4.8-dev libdb4.8++-dev

WORKDIR /work
COPY . .

RUN ./autogen.sh
RUN ./configure --disable-tests --disable-bench
RUN make -j8
RUN make install #optional
ENV PATH="/work/src:${PATH}"

RUN useradd -ms /bin/bash firovm
USER firovm

RUN mkdir -p /home/firovm/.fvm/
RUN chown firovm:firovm /home/firovm/.fvm/
RUN cp /work/fvm.regtest.conf /home/firovm/.fvm/fvm.conf

CMD [ "firovmd", "--daemon=0", "--printtoconsole", "--rpcbind=0.0.0.0", "--rpcallowip=0.0.0.0/0" ]