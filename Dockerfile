FROM ubuntu:jammy

RUN apt-get update  && apt-get upgrade -y

RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 871920D1991BC93C

RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 871920D1991BC93C || true

RUN apt-get update || true

RUN apt-get install -y curl bash libreadline-dev file binutils

RUN curl -fsSL -o /install.sh https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh

RUN chmod +x install.sh

WORKDIR /

CMD ["bash", "/install.sh"]