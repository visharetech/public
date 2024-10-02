# Use the official Ubuntu 22.04 image from Docker Hub
FROM ubuntu:22.04


RUN apt update && apt install -y build-essential cmake git wget gcc-multilib g++-multilib libclang-dev clang python3 python3-pip nano sudo &&    \
    rm -rf /var/lib/apt/lists/*

RUN wget -O riscv32-toolchain.tar.gz https://github.com/stnolting/riscv-gcc-prebuilt/releases/download/rv32i-4.0.0/riscv32-unknown-elf.gcc-12.1.0.tar.gz && \
    mkdir -p /opt/riscv &&  \
    tar -xzf riscv32-toolchain.tar.gz -C /opt/riscv/ && \
    rm -f riscv32-toolchain.tar.gz

# Download asim
RUN wget -O asim https://github.com/frankvisharegrp/public/releases/download/v1.0/asim && \
    mkdir -p /opt/asim &&   \
    cp asim /opt/asim/ &&   \
    rm -f asim

# Create a new user 'vishare' with password 'vishare' and add to sudoers
RUN useradd -m -s /bin/bash vishare && \
    echo 'vishare:vishare' | chpasswd && \
    echo 'vishare ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

RUN chown -R vishare:vishare /opt/riscv && \
    chown -R vishare:vishare /opt/asim && \
    chmod +x /opt/asim/asim

# Switch to the new user
USER vishare

# Set the working directory
WORKDIR /home/vishare

RUN touch ~/.bashrc && \
    echo 'export PATH=$PATH:/opt/riscv/bin:/opt/asim' >> ~/.bashrc

# Specify the command to run on container start
CMD ["bash"]

