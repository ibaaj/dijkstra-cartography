FROM debian:jessie

ENV DEBIAN_FRONTEND noninteractive


RUN apt-get -y update
RUN apt-get -y install libboost-dev \
      build-essential \
      gcc \
      g++ \
      make \
      cmake \
      libstxxl-dev \
      libxml2-dev \
      libbz2-dev \
      zlib1g-dev \
      libzip-dev \
      libboost-filesystem-dev \
      libboost-thread-dev \
      libboost-system-dev \
      libboost-regex-dev \
      libboost-program-options-dev \
      libboost-iostreams-dev \
      libgomp1 \
      liblua5.1-0-dev \
      libluabind-dev \
      pkg-config \
      libtbb-dev \
      libboost-test-dev;

COPY osrm-contract /
COPY osrm-datastore /
COPY osrm-extract /
COPY osrm-routed /
COPY planet-railway.osm /
COPY rail.lua /

RUN ./osrm-extract -p rail.lua planet-railway.osm;
RUN ./osrm-contract planet-railway.osrm;


RUN apt-get clean \
 && rm -rf /var/lib/apt/lists/*

COPY run.sh /
RUN chmod +x /run.sh
ENTRYPOINT ["/run.sh"]

EXPOSE 5000
