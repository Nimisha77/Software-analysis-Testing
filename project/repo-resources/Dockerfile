FROM ubuntu:20.04

ENV HOME=/home
WORKDIR ${HOME}

# Dependencies
ENV SVF_PROJECT_DEPS="cmake make gcc g++ libncurses5-dev libssl-dev libpcre2-dev zip vim curl tar wget xz-utils python git gdb nano zlib1g-dev unzip libtinfo5 nodejs npm"
RUN apt-get update 
RUN DEBIAN_FRONTEND=noninteractive TZ=America/New_York apt-get -y install ${SVF_PROJECT_DEPS}

# SVF & LLVM binary
RUN npm i --silent svf-lib --prefix ${HOME}

# Project Infrastructure
RUN git clone https://github.com/davis-matthew/SVF-Project.git
RUN mv SVF-Project project