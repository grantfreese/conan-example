# docker container for testing bcu build
#
# to build:
# docker build -t testcontainer .
#
# to run:
# docker run --rm -it --volume=/home/${USER}/.ssh:/root/.ssh testcontainer
#


FROM debian:bullseye

RUN mkdir -p /home/user

RUN DEBIAN_FRONTEND=noninteractive apt update \
    && apt install -y \
        binutils \
        build-essential \
        gcc-10 \
        g++-10 \
        git \
        python3-pip

# make sure we're using gcc-10
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20 --slave /usr/bin/g++ g++ /usr/bin/g++-10

RUN pip3 install conan parktoma-vscconan

CMD ["bash"]