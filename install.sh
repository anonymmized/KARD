#!/usr/bin/env bash
set -euo pipefail

REPO_URL="https://github.com/<you>/KARD.git"
BRANCH="main"
INSTALL_DIR="${HOME}/.local/share/kard/src"

log() { printf '[install] %s\n' "$*"; }
die() { printf '[install] error: %s\n' "$*" >&2; exit 1; }

detect_pm() {
  for pm in brew apt-get dnf yum pacman zypper apk; do
    if command -v "$pm" >/dev/null 2>&1; then echo "$pm"; return 0; fi
  done
  return 1
}

pm_install() {
  local pm="$1"; shift
  case "$pm" in
    brew)    brew install "$@" ;;
    apt-get) sudo apt-get install -y "$@" ;;
    dnf)     sudo dnf install -y "$@" ;;
    yum)     sudo yum install -y "$@" ;;
    zypper)  sudo zypper --non-interactive install "$@" ;;
    pacman)  sudo pacman -S --noconfirm "$@" ;;
    apk)     sudo apk add "$@" ;;
    *)       die "unsupported package manager: $pm" ;;
  esac
}

pkg_names() {
  case "$1" in
    brew)    echo "git cmake cpr nlohmann-json" ;;
    apt-get) echo "git cmake build-essential libcpr-dev nlohmann-json3-dev" ;;
    pacman)  echo "git cmake base-devel cpr nlohmann-json" ;;
    apk)     echo "git cmake build-base cpr nlohmann-json" ;;
    dnf|yum) echo "git cmake gcc-c++ make nlohmann-json-devel" ;;
    zypper)  echo "git cmake gcc-c++ make nlohmann_json-devel" ;;
    *)       die "no package list for $1" ;;
  esac
}

main() {
  log "detecting package manager..."
  PM="$(detect_pm)" || die "no supported package manager found"
  log "using: $PM"

  if [ "$PM" != "brew" ]; then sudo -v || die "sudo required"; fi

  log "installing dependencies..."
  pm_install "$PM" $(pkg_names "$PM")

  log "fetching source -> $INSTALL_DIR"
  if [ -d "$INSTALL_DIR/.git" ]; then
    git -C "$INSTALL_DIR" pull --ff-only
  else
    mkdir -p "$(dirname "$INSTALL_DIR")"
    git clone --depth 1 -b "$BRANCH" "$REPO_URL" "$INSTALL_DIR"
  fi

  log "building..."
  cmake -S "$INSTALL_DIR" -B "$INSTALL_DIR/build"
  cmake --build "$INSTALL_DIR/build"

  log "done -> $INSTALL_DIR/build/kard"
}

main "$@"
