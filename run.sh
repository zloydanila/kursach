#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# Create .env if missing
if [ ! -f .env ]; then
  cp .env.example .env
fi

# Auto-fill technical vars needed by docker-compose (volumes/user mapping)
grep -q "^UID=" .env 2>/dev/null || echo "UID=$(id -u)" >> .env
grep -q "^DOWNLOADS_DIR=" .env 2>/dev/null || echo "DOWNLOADS_DIR=$HOME/Downloads" >> .env

# Start
docker compose up --build -d
docker compose ps
