#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

die() { echo "ERROR: $*" >&2; exit 1; }

command -v docker >/dev/null 2>&1 || die "docker is not installed"
docker compose version >/dev/null 2>&1 || die "docker compose plugin is not available"

# Create .env if missing
if [ ! -f .env ]; then
  [ -f .env.example ] || die ".env.example not found"
  cp .env.example .env
  echo "Created .env from .env.example"
fi

# Auto-fill vars used by docker-compose
grep -q "^UID=" .env 2>/dev/null || echo "UID=$(id -u)" >> .env
grep -q "^DOWNLOADS_DIR=" .env 2>/dev/null || echo "DOWNLOADS_DIR=$HOME/Downloads" >> .env

# Validate DB password
POSTGRES_PASSWORD="$(grep -E "^POSTGRES_PASSWORD=" .env | tail -n 1 | cut -d= -f2- || true)"
[ -n "${POSTGRES_PASSWORD}" ] || die "POSTGRES_PASSWORD is empty in .env"
[ "${POSTGRES_PASSWORD}" != "change_me" ] || die "POSTGRES_PASSWORD is still change_me. Edit .env and set a real password."

# Validate compose interpolation early
docker compose config >/dev/null

docker compose up --build -d
docker compose ps
echo "OK: project is up"
