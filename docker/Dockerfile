# Use the official Ubuntu 22.04 image from Docker Hub
FROM ubuntu:22.04

ARG DOCKER_USER=vishare
ARG DOCKER_USER_PASSWORD=vishare

RUN apt update && apt install -y build-essential cmake git wget gcc-multilib g++-multilib libclang-dev clang python3 python3-pip nano &&    \
    rm -rf /var/lib/apt/lists/*

RUN wget -O riscv32-toolchain.tar.gz https://github.com/stnolting/riscv-gcc-prebuilt/releases/download/rv32i-4.0.0/riscv32-unknown-elf.gcc-12.1.0.tar.gz && \
    mkdir -p /opt/riscv &&  \
    tar -xzf riscv32-toolchain.tar.gz -C /opt/riscv/ && \
    rm -f riscv32-toolchain.tar.gz

# Download asim
RUN wget -O asim https://github.com/visharetech/public/releases/download/v1.0/asim && \
    mkdir -p /opt/asim &&   \
    cp asim /opt/asim/ &&   \
    rm -f asim

# Add the user
RUN groupadd $DOCKER_USER && useradd -m -g $DOCKER_USER -s /bin/bash $DOCKER_USER

RUN echo "$DOCKER_USER:$DOCKER_USER_PASSWORD" | chpasswd

# Change the ownership
RUN chown -R $DOCKER_USER:$DOCKER_USER /opt/riscv && \
    chown -R $DOCKER_USER:$DOCKER_USER /opt/asim && \
    chmod +x /opt/asim/asim

# Switch to the new user
USER $DOCKER_USER

# Set the working directory
WORKDIR /home/$DOCKER_USER

RUN touch ~/.bashrc && \
    echo 'export PATH=$PATH:/opt/riscv/bin:/opt/asim' >> ~/.bashrc

# wget the asim_helloworld_example and extract to home directory
RUN wget -O asim_helloworld_example.tar.gz https://github.com/visharetech/public/releases/download/v1.0/asim_helloworld_example.tar.gz && \
    tar -xzf asim_helloworld_example.tar.gz -C ~ && \
    rm -f asim_helloworld_example.tar.gz

# Specify the command to run on container start
CMD ["bash"]

