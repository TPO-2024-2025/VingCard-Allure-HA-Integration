# Use slim Python image
FROM python:3.13-slim

# Set environment variables
ENV PYTHONUNBUFFERED=1 \
    PIP_NO_CACHE_DIR=1

# Set working directory
WORKDIR /usr/src/app

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    build-essential \
    libffi-dev \
    libssl-dev \
    libjpeg-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    libxml2-dev \
    libxmlsec1-dev \
    liblzma-dev \
 && apt-get clean && rm -rf /var/lib/apt/lists/*

# Clone the Home Assistant repo and checkout the tag
RUN git clone https://github.com/home-assistant/core.git /usr/src/app/homeassistant \
 && cd /usr/src/app/homeassistant \
 && git checkout tags/2024.12.5

# Copy your configuration and custom integration
COPY config/ /config/

# Install dependencies and Home Assistant in editable mode
RUN pip install -U pip setuptools wheel \
 && pip install -r /usr/src/app/homeassistant/requirements.txt \
 && pip install -e /usr/src/app/homeassistant[all]

# Expose port for Home Assistant
EXPOSE 8123

# Start Home Assistant
CMD ["hass", "--config", "/config"]